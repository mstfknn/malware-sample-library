#pragma once

#include "core.h"

#define SIO_RCVALL         0x98000001

typedef struct IPHeader
{
	UCHAR   iph_verlen;   // версия и длина заголовка
	UCHAR   iph_tos;      // тип сервиса
	USHORT  iph_length;   // длина всего пакета
	USHORT  iph_id;       // Идентификация
	USHORT  iph_offset;   // флаги и смещения
	UCHAR   iph_ttl;      // время жизни пакета
	UCHAR   iph_protocol; // протокол
	USHORT  iph_xsum;     // контрольная сумма
	ULONG   iph_src;      // IP-адрес отправителя
	ULONG   iph_dest;     // IP-адрес назначения
} IPHeader;


namespace Sniffer
{

//если возвращает true то фильтрация заканчивается
typedef bool (*typeIPPacket)( const IPHeader* header, const void* data, int c_data, void* tag );

//фильтрует пакеты по исходящим ipSrc и входящим ipDst айпи адресам указанного протокола protocol
//если c_ipSrc = 0, то исходящие IP адреса не фильтруются (передаются все)
//если c_ipDst = 0, то входящие IP адреса не фильтруются (передаются все)
//если protocol = 0, то передаются все протоколы
//wait - время ожидание пакета, если время ожидание истекло, то в func передается адреса пакета и данных равным 0
//фильтрация заканчивается если func возвращает true
void Filter( ULONG* ipSrc, int c_ipSrc, ULONG* ipDst, int c_ipDst, UCHAR protocol, typeIPPacket func, int wait, void* tag );

}
