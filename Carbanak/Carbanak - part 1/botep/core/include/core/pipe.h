//каналы для межпроцессорного обмена

#pragma once

#include "core.h"
#include "misc.h"

class PipeResponse;

namespace Pipe
{

struct Msg;
typedef MovedPtr<Msg> AutoMsg;

//тип функции которая получает ответ на запрос по pipe каналу
typedef void (*typeReceiverPipeAnswer)( AutoMsg msg, DWORD tag );

//отключает предупреждение на массив нулевого размера
#pragma warning ( disable : 4200 )
#pragma pack(1)
struct Msg
{
	int cmd; //ид сообщения
	char answer[32]; //какому каналу возвращать ответ
	typeReceiverPipeAnswer func; //которой нужно передать ответ
	DWORD tag; //значение которое возвращается для func, для запросов с ответами иногда нужно передать дополнительный параметр здесь его и передаем
	int sz_data; //длина данных, идущих с сообщением
	byte data[0];
};
#pragma pack()

//отсылка ответа каналу указанному в msg
bool SendAnswer( const Msg* msg, int cmd, const void* data, int sz_data );

extern PipeResponse* serverPipeResponse;
//инициализация сервера приема сообщений по каналу, который отсылает их указанным функциям
bool InitServerPipeResponse();

}

class PipePoint
{
	protected:

		HANDLE pipe;
		StringBuilderStack<32> remote;
		StringBuilderStack<32> name;
		bool onlyReadOrWrite; //true - канал одноправленный, для сервера только чтение, для клиента только запись

		bool Read( Mem::Data& data );
		//отправка данных в канал, данные шифруются поэтому массив data будет изменен
		bool Write( const void* data, int c_data );
		StringBuilder& GetFullName( StringBuilder& fullName ); //создает и возвращает полное имя канала

	public:

		PipePoint();
		~PipePoint();
		const StringBuilder& GetName() const
		{
			return name;
		}
		void SetName( const char* _name ); //устанавливает имя канала
		static void GenName( StringBuilder& newName ); //генерирует имя канала
		void Close();
		bool IsValid()
		{
			return pipe != nullptr;
		}
};

class PipeServer : public PipePoint
{
	protected:
		//размеры буферов для чтения и записи канала
		static const int SizeInBuffer = 1024;
		static const int SizeOutBuffer = 1024;

		//обработчик принятых сообщений, если возвращает размер ответа, то отсылает сразу данные из msgOut, если 0 - то ответа нет, если -1, то сервер должен завершить работу
		virtual int Handler( Pipe::Msg* msgIn, void** msgOut );
		//вызывается при получении команды CmdDisconnect, после чего сервер завершает работу
		virtual void Disconnect();
		//цикл приема сообщений
		void Loop();
		//передача сообщения обработчику который запускается в отдельном потоке, при этом сообщение дублируется
		//это используется если обработка сообщения занимает долгое время, а канал нужно освобождать как можно быстрее
		static bool HandlerAsync( Pipe::typeReceiverPipeAnswer func, Pipe::Msg* msg, DWORD tag = 0 );

	public:

		enum
		{
			CmdDisconnect = 1000000 // команда на отключение (завершение) сервера, по этой команде вызывается функция Disconncet()
		};

		PipeServer();
		~PipeServer();
		bool Start( bool onlyRead = true ); //запускает канал на прием и обработку данных, выходит только после закрытия канала
		bool StartAsync( bool onlyRead = true ); //запускает канал в отдельном потоке
		void StartAnotherThread() //запуск сервера после входа в другой поток (после вызова StartAsync с учетом аргумента onlyRead)
		{
			Start(onlyReadOrWrite);
		}
		void Stop(); //остановка сервера, нельзя вызывать в том же потоке что и сам сервер (при обработке сообщений в Handler())
};

//получает ответы и отправляет их указанным функциям (typereceiverPipeAnswer). Функции запускаются в отдельных потоках
//чтобы не блокировать этот сервер
class PipeResponse : public PipeServer
{

		virtual int Handler( Pipe::Msg* msgIn, void** msgOut );

	public:

		PipeResponse();
		~PipeResponse();
};

class PipeClient : public PipePoint
{

	protected:

	public:

		PipeClient( const char* _name ); //name - имя канала которому нужно передать сообщение
		~PipeClient();
		bool Connect( bool onlyWrite = true ); //связывается с каналом
		//пишет в канал данные
		bool Write( int cmd, const void* data, int sz_data, const char* nameReceiver = 0, Pipe::typeReceiverPipeAnswer funcReceiver = 0, DWORD tag = 0 );
		//посылает запрос и получает сразу ответ в result
		//При выполнении запроса канал открывается и после закрывается
		bool PipeClient::Request( int cmd, const void* data, int sz_data, Mem::Data& result );
		//отсылает данные, аналогична Write, только еще делает Connect() и и после Close()
		bool Send( int cmd, const void* data, int sz_data, const char* nameReceiver = 0, Pipe::typeReceiverPipeAnswer funcReceiver = 0, DWORD tag = 0 );
		bool Send( const void* data, int sz_data );
		//отсылка сообщения указанному каналу
		static bool Send( const char* namePipe, int cmd, const void* data, int sz_data, const char* nameReceiver = 0, Pipe::typeReceiverPipeAnswer funcReceiver = 0, DWORD tag = 0 );
		static bool Send( const char* namePipe, const void* data, int sz_data );
};
