#pragma once

#include "socket.h"
#include "core.h"
#include "proxy.h"

namespace HTTP
{

class Request
{
		StringBuilderStack<128> host; //имя хоста без http
		StringBuilder file; //все что идет после имени хоста в урле
		int port; //номер порта, по умолчанию 80
		Mem::Data data; //полученные в результате запроса данные
		Mem::Data postData; //отправляемые данные при POST запросе
		StringBuilder contentType; //тип отправляемых данных

		//информация присланного заголовка
		int answerCode; //код ответа
		int contentLen; //длина присланных данных, -1 - длина неизвестна (не указана)
		StringBuilder location; //редирект
		int transferEncoding; //если 1 (chunked), то ответ собираем из присланных частей
		Proxy::Connector* cn; //коннектор через который идет соединение

	public:

		enum Protocol
		{
			HTTP, HTTPS
		};

	private:

		Protocol protocol;

		//формирование заголовка запроса
		bool CreateHeader( StringBuilder& header, bool post, bool proxy, const StringBuilder& authentication );
		//общение с сервером, отсылка запроса и получение ответа
		bool SendRecv( const char* type, int wait, bool post );
		//выполнение запроса указанного типа
		bool Exec( const char* type, int wait, bool post );
		//анализирует присланный заголовок и заполняет нужные поля класса
		bool AnalysisHeader( const StringBuilder& header );
		void Init( Protocol _protocol, Proxy::Connector* _cn = 0 );

	public:

		Request( Protocol _protocol = HTTP, Proxy::Connector* cn = 0 );
		Request( Proxy::Connector* cn );
		~Request();

		void SetHost( const char* _host ); //установка хоста
		//где-то стоит макрос SetPort, убираем его чтобы имя функции не подменялось
		#undef SetPort
		void SetPort( int _port )
		{
			port = _port;
		}
		void SetFile( const char* _file ); //установка части урла идущего после хоста
		bool SetUrl( const char* url ); //анализирует урл и разбивает его на имя хоста и файла, если все верно, то возвращает true
		//выполняет запрос GET, wait - время ожидания ответа в миллисекундах
		bool Get( int wait = 0 ); 
		//выполняет запрос POST, wait - время ожидания ответа в миллисекундах
		bool Post( int wait = 0 ); 
		StringBuilder& GetUrl( StringBuilder& url ) const;
		void NameProtocol( StringBuilder& name ) const; //имя протокола для формирования урла
		void VerProtocol( StringBuilder& name ) const; //имя и версия протокола для формирования заголовка запроса
		bool AddPostData( const void* ptr, int c_ptr ) //добавляет данные для POST запроса
		{
			return postData.Append( ptr, c_ptr );
		}
		bool AddPostData( const StringBuilder& s )
		{
			return postData.Append( s.c_str(), s.Len() );
		}
		bool AddPostData( const Mem::Data& data )
		{
			return postData.Append( data.Ptr(), data.Len() );
		}

		Mem::Data& Response() //полученные данные (ответ)
		{
			return data;
		}
		void SetContentType( const char* s );
		void SetContentWebForm();
		void SetContentMultipart( const char* boundary );
		int AnswerCode() const
		{
			return answerCode;
		}
};

//добавляет в запрос пост данные в формате Multipart
class PostDataMultipart
{
		Request& request;
		StringBuilderStack<64> boundary;
		StringBuilderStack<8> end; //символ конца строки

		//добавляет имя поля
		void AddName( StringBuilder& form, const char* name );

	public:

		PostDataMultipart( Request& _request );
		//добавляет пару name = value
		void AddValue( const char* name, const char* value );
		//добавляет файл, содержимое файла должно быть в data
		void AddFile( const char* name, const char* fileName, const void* data, int c_data );
		void AddFile( const char* name, const char* fileName, Mem::Data& data )
		{
			AddFile( name, fileName, data.Ptr(), data.Len() );
		}
		//завершение добавления данных
		void End();
};

//кодирует строку таким образом, чтобы были только допустимые символы для урла
StringBuilder& UrlEncode( const char* src, StringBuilder& dst );

}
