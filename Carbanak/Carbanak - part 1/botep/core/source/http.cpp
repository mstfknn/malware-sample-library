#include "core\socket.h"
#include "core\http.h"
#include "core\file.h"
#include "core\debug.h"
#include "core\rand.h"

#include <urlmon.h>

namespace HTTP
{

void Request::Init( Protocol _protocol, Proxy::Connector* _cn )
{
	port = HTTP_Port;
	protocol = _protocol;
	cn = _cn;
}

Request::Request( Protocol _protocol, Proxy::Connector* cn )
{
	Init( _protocol, cn );
}

Request::Request( Proxy::Connector* cn )
{
	Init( HTTP, cn );
}

Request::~Request()
{
}

void Request::SetHost( const char* _host )
{
	host = _host;
}

void Request::SetFile( const char* _file )
{
	file = _file;
}

bool Request::SetUrl( const char* url )
{
	int p = Str::IndexOf( url, ':' );
	if( p >= 0 ) //найден http://
	{
		if( p >= 3 && p < 8 ) //проверяем действительно ли и так
		{
			if( (url[p + 1] == '/' && url[p + 2] == '/') || (url[p + 1] == '\\' && url[p + 2] == '\\') )
			{
				url += p + 3;
			}	
			else
				return false;
		}
		else
			return false;
	}
	//здесь url указывает на имя хоста
	p = Str::IndexOf( url, '/' );
	if( p < 0 ) p = Str::IndexOf( url, '\\' );
	if( p < 0 ) //есть только имя хоста
	{
		host = url;
		file.SetEmpty();
	}
	else
	{
		host.Copy( url, p );
		file = url + p + 1; //все что после имени хоста
	}
	return true;
}

void Request::NameProtocol( StringBuilder& name ) const
{
	switch( protocol )
	{
		case HTTP: name = _CS_("http"); break;
		default:
			name.SetLen(0); //делаем пустую строку
	}
}

void Request::VerProtocol( StringBuilder& name ) const
{
	switch( protocol )
	{
		case HTTP: name = _CS_("HTTP/1.1"); break;
		default:
			name.SetLen(0); //делаем пустую строку
	}
}

StringBuilder& Request::GetUrl( StringBuilder& url ) const
{
	NameProtocol(url);
	url += ':'; url += '/'; url += '/';
	url += host;
	if( !file.IsEmpty() )
	{
		url += '/';
		url += file;
	}
	return url;
}

bool Request::CreateHeader( StringBuilder& header, bool post, bool proxy, const StringBuilder& authentication )
{
	//конец строки в загловке HTTP запроса
	StringBuilderStack<3> end; end.FillEndStr();
	header += ' ';
	if( !proxy )
	{
		//1-я строка заголовка: GET /file HTTP/1.1
		header += '/';
		header += file; header += ' ';
	}
	else //для прокси нужно указывать полный урл
	{
		StringBuilder url;
		header += GetUrl(url);
		header += ' ';
	}
	StringBuilderStack<32> verProtocol;
	VerProtocol(verProtocol);
	header += verProtocol;
	header += end;
	//2-я строка заголовка: Host: host
	header += _CS_("Host: ");
	header += host;
	header += end;
	//3-я строка заголовка: User-Agent: .....
	StringBuilderStack<256> userAgent;
	DWORD szUserAgent = userAgent.Size();
	if( API(URLMON, ObtainUserAgentString)( 0, userAgent.c_str(), &szUserAgent ) != NOERROR ) return false;
	userAgent.UpdateLen();
	header += _CS_("User-Agent: ");
	header += userAgent;
	header += end;
	//остальные строки
	header += _CS_("Accept: */*");
	header += end;
	if( post )
	{
		header += _CS_("Content-Length: ");
		char buf[16];
		int len = Str::ToString( postData.Len(), buf );
		header.Cat( buf, len );
		header += end;
	}
	if( !contentType.IsEmpty() )
	{
		header += _CS_("Content-Type: ");
		header += contentType;
		header += end;
	}
	if( proxy && !authentication.IsEmpty() )
	{
		header += _CS_("Proxy-Authorization: ");
		header += authentication;
		header += end;
	}
	header += end; //конец заголовка
	return true;
}

bool Request::SendRecv( const char* type, int wait, bool post )
{
	bool ret = false;
	int sc = 0;
	StringBuilder authentication;
	bool proxy = false;
	if( cn )
		sc = cn->Connect( host, port, proxy, authentication );
	else
		sc = Socket::ConnectHost( host, port );
	if( sc > 0 )
	{
		StringBuilder header( 1024, type );
		if( CreateHeader( header, post, proxy, authentication ) )
		{
			if( Socket::Write( sc, header.c_str(), header.Len() ) > 0 ) //отправляем заголовок
			{
				bool errPost = false;
				if( post && postData.Len() > 0 )
				{
					if( Socket::Write( sc, postData.Ptr(), postData.Len() ) <= 0 ) //отправляем данные для post запроса
						errPost = true;
				}
				if( !errPost )
				{
					//читаем до тех пор пока не считаем заголовок
					int p = 0;
					int res = 0;
					char endHeader[5];
					endHeader[0] = endHeader[2] = '\r';
					endHeader[1] = endHeader[3] = '\n';
					endHeader[4] = 0;
					for(;;)
					{
						res = Socket::Read( sc, data, -1, wait );
						if( !res ) break;
						int p2 = data.IndexOf( p, endHeader, 4 );
						if( p2 >= 0 ) //прочли заголовок
						{
							p = p2; //конец заголовка
							break;
						}
						if( res == 2 ) //больше данных не будет и заголовок не полный, считаем ошибкой
						{
							res = 0;
							break;
						}
						p = data.Len() - 4; //позиция с которой нужно будет проверять снова конец заголовка, вычитаем 4, так как конец заголовка может быть разорван, т. е. часть в конце считанного массива, а часть в начале следующей порции
					}
					if( res )
					{
						StringBuilder header( data, 0, p );
						if( AnalysisHeader(header) )
						{
							p += 4; //начало данных
							data.Submem(p); //отбрасываем заголовок, оставляем только данные
							if( res == 1 ) //есть еще данные
							{
								int read = 0;
								if(	contentLen > 0 ) read = contentLen - data.Len();
								if( read > 0 )
									res = Socket::Read( sc, data, read, wait );
							}
						}
						if( res ) ret = true;
					}
				}
			}
		}
		Socket::Close(sc);
	}
	return ret;
}

//в таком формате сначала идет строка с длиною (в hex формате), следующая строка с данными указанной длины, и т.д.
//конец считается если длина = 0
static void ChunkedToNormal( Mem::Data& data )
{
	int to = 0, from = 0;
	char* ptr = (char*)data.Ptr();
	for(;;)
	{
		int size = Str::ToInt( (char*)data.Ptr() + from, true );
		if( size == 0 ) break;
		while( ptr[from++] != '\r' );
		if( ptr[from] < ' ' ) from++; //проходим \n
		Mem::Copy( ptr + to, ptr + from, size );
		to += size;
		from += size + 1; //+1 это \r
		if( ptr[from] < ' ' ) from++; //проходим \n
	}
	data.SetLen(to);
}

bool Request::Exec( const char* type, int wait, bool post )
{
	bool ret = false;
	data.Clear();
	if( SendRecv( type, wait, post ) )
	{
		if( transferEncoding == 1 )
			ChunkedToNormal(data);
		ret = true;
	}
	return ret;
}

bool Request::AnalysisHeader(  const StringBuilder& header )
{
	//устанавливаем значения по умолчанию
	answerCode = 0;
	contentLen = -1;
	location.SetEmpty();
	transferEncoding = 0;

	char delim[3]; delim[0] = '\r'; delim[1] = '\n'; delim[2] = 0;
	StringArray ss = header.Split( delim, 2 );
	if( ss.Count() < 2 ) return false;
	//в 1-й строке код ответа
	int p = ss[0]->IndexOf(' ');
	if( p < 0 ) return false;
	answerCode = ss[0]->ToInt( p + 1 );
	StringBuilderStack<64> param; //имя параметра
	for( int i = 1; i < ss.Count(); i++ )
	{
		p = ss[i]->IndexOf(':');
		if( p > 0 )
		{
			StringBuilder& s = *ss[i];
			param.Copy( s, p );
			param.Lower();
			p++;
			while( s[p] == ' ' ) p++;
			if( param == _CS_("content-length") )
			{
				contentLen = s.ToInt(p);
			}
			else if( param == _CS_("location") )
			{
				location.Substring( s, p );
			}
			else if( param == _CS_("transfer-encoding") )
			{
				if( Str::Cmp( s.c_str() + p, _CS_("chunked") ) == 0 )
					transferEncoding = 1;
			}
		}
	}
	return true;
}

bool Request::Get( int wait )
{
	return Exec( _CS_("GET"), wait, false );
}

bool Request::Post( int wait )
{
	return Exec( _CS_("POST"), wait, true );
}

void Request::SetContentType( const char* s )
{
	contentType += s;
}

void Request::SetContentWebForm()
{
	SetContentType( _CS_("application/x-www-form-urlencoded" ) );
}

void Request::SetContentMultipart( const char* boundary )
{
	SetContentType( _CS_("multipart/form-data; boundary=") );
	contentType += boundary;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

StringBuilder& UrlEncode( const char* src, StringBuilder& dst )
{
	dst.SetEmpty();
	if( src )
	{
		char c;
		while( c = *src++ )
		{
			if( (c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') )
			{
				dst += c;
			}
			else
			{
				dst += '%';
				dst += Str::DecToHex( (byte)c >> 4 );
				dst += Str::DecToHex( c & 0xf );
			}
		}
	}
	return dst;
}

PostDataMultipart::PostDataMultipart( Request& _request ) : request(_request)
{
	boundary.Set( '-', 28 );
	StringBuilderStack<16> id;
	Rand::Gen( id, 12 );
	boundary += id;
	request.SetContentMultipart(boundary);
	end.FillEndStr();
}

void PostDataMultipart::AddName( StringBuilder& form, const char* name )
{
	form += '-'; 
	form += '-';
	form += boundary;
	if( name )
	{
		form += end;
		form += _CS_("Content-Disposition: form-data; name=\"");
		form += name;
		form += '"';
	}
}

void PostDataMultipart::AddValue( const char* name, const char* value )
{
	StringBuilder form;
	AddName( form, name );
	form += end;
	form += end;
	form += value;
	form += end;
	request.AddPostData(form);
}

void PostDataMultipart::AddFile( const char* name, const char* fileName, const void* data, int c_data )
{
	StringBuilder form;
	AddName( form, name );
	form += ';';
	form += _CS_(" filename=\"");
	form += fileName;
	form += '"';
	form += end;
	form += _CS_("Content-Type: application/octet-stream");
	form += end;
	form += end;
	request.AddPostData(form);
	request.AddPostData( data, c_data );
	request.AddPostData( end.c_str(), end.Len() );

}

void PostDataMultipart::End()
{
	StringBuilder form;
	AddName( form, 0 );
	form += '-'; 
	form += '-';
	form += end;
	request.AddPostData(form);
}

}

