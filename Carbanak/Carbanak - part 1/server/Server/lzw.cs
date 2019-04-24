using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Server
{
    class LZW
    {
        struct code
        {
	        public short prefix;
	        public byte sym;
	        public short next;
        };

        //состояние вывода в поток или ввода из потока
        struct StateLZW
        {
	        public byte[] buf; //буфер куда пишем (читаем) данные
            public int p; //позиция в буфере
	        public short maxCode; //текущий максимальный код
	        public int bs; //сколько бит занятов в байте
        };

        //таблица кодов, коды от 0 до 256 начальные символы (256 - код окончания файла)
        code[] codes;

        const int CODE_END = 256;
        const int CODE_CLEAN = 257; //код очистки таблицы
        const int CODE_BEG = 258; //начальный код

        public LZW()
        {
	        if( codes == null )
		        codes = new code[4096]; //таблица на 4096 кодов
            init();
        }

        void init()
        {
	        for( int i = 0; i <= 256; i++ )
	        {
		        codes[i].prefix = -1;
		        codes[i].sym = (byte)i;
		        codes[i].next = -1;
	        }
        }

        int output_code( int code, ref StateLZW st )
        {
        	int bits;
	        if( st.maxCode > 2048  )
		        bits = 12;
	        else
		        if( st.maxCode > 1024 )
			        bits = 11;
		        else
			        if( st.maxCode > 512 )
				        bits = 10;
			        else
				        bits = 9;
	        st.buf[st.p++] |= (byte)(code << st.bs); 
	        code >>= (8 - st.bs);
	        bits -= 8 - st.bs;
	        if( bits > 8 )
	        {
		        st.buf[st.p++] = (byte)code;
		        bits -= 8;
		        code >>= 8;
	        }
	        st.buf[st.p] = (byte)code;
	        st.bs = bits;
            return st.p;
        }

        int input_code( ref StateLZW st )
        {
	        int bits;
            int maxCode = st.maxCode + 1;
	        if( maxCode > 2048  )
		        bits = 12;
	        else
		        if( maxCode > 1024 )
			        bits = 11;
		        else
			        if( maxCode > 512 )
				        bits = 10;
			        else
				        bits = 9;
	        int code = st.buf[st.p++] >> st.bs;
	        int bs2 = (8 - st.bs);
	        bits -= bs2;
	        if( bits > 8 ) 
	        {
		        code |= st.buf[st.p++] << bs2;
		        bs2 += 8;
		        bits -= 8;
	        }
	        code |= (st.buf[st.p] & ((1 << bits) - 1)) << bs2;
	        st.bs = bits;
	        return code;
        }


        public int to_lzw( byte[] from, int c_from, byte[] to )
        {
	        int s = -1;
	        StateLZW st;
	        st.buf = to;
            st.p = 0;
	        st.maxCode = CODE_BEG;
	        st.bs = 0;
	        byte c = 0;
	        init();
	        int passed = 0; //пройдено байтов
	        int percent = 5; //эффективный процент сжатия
	        to[0] = 0;
	        while( passed < c_from )
	        {
		        int sn, sp = 0;
		        c = from[passed]; passed++;
        		if( s < 0 )
			        sn = c;
		        else
		        {
			        sp = s;
			        sn = codes[s].next; //коды с префиксом s
			        while( sn >= 0 )
			        {
				        if( codes[sn].sym == c ) break;
				        sp = sn;
				        sn = codes[sn].prefix;
			        }
		        }
		        if( sn >= 0 ) //такая строка есть
		        {
			        s = sn;
		        }
		        else  //строка не обнаружена
		        {
			        output_code( s, ref st );
			        if( sp == s ) //таких префиксов еще нет, создаем
			        {
				        codes[sp].next = st.maxCode;
			        }
        			else 
			        {
				        codes[sp].prefix = st.maxCode;
				        codes[sp].next = -1;
			        }
			        codes[st.maxCode].prefix = -1;
			        codes[st.maxCode].sym = c;
			        codes[st.maxCode].next = -1;
			        st.maxCode++;
			        if( st.maxCode >= 4096 ) //достигли максимального значения кодов
			        {
				        output_code( CODE_CLEAN, ref st );
				        init();
				        st.maxCode = CODE_BEG;
			        }
			        s = c;
		        }
		        if( (passed % 200) == 0 ) //через каждые 200 байт проверяем эффективность
		        {
			        int saved = st.p + 1;
			        if( passed < st.p ) return -1; //записано больше чем считано
			        if( (passed - saved) * 100 / passed < percent ) return -1; //не эффективное сжатие
			        percent += percent;
			        if( percent > 10 ) percent = 10;
		        }
	        }
	        output_code( s, ref st );
	        output_code( CODE_END, ref st );
	        return st.p + 1;
        }

        public int from_lzw( byte[] from, byte[] to )
        {
            int c, oc = -1, s;
	        StateLZW st;
	        st.buf = from;
            st.p = 0;
	        st.maxCode = CODE_BEG;
	        st.bs = 0;
	        init();
            int saved = 0;
            int loops = 0;
        	while( true ) 
	        {
		        c = input_code(ref st);
		        if( c == CODE_END ) break;
		        if( c == CODE_CLEAN )
		        {
			        init();
			        st.maxCode = CODE_BEG;
			        oc = -1;
			        continue;
		        }
		        if( c < st.maxCode ) 
		        {
			        s = c;
			        codes[s].next = -1;
                    
                    loops = 2 * codes.Length;
			        while( codes[s].prefix >= 0 && loops > 0 )
			        {
                        loops--;
				        codes[codes[s].prefix].next = (short)s;
				        s = codes[s].prefix;
			        }
                    if (loops <= 0)
                    {
                        saved = -1;
                        break;
                    }

			        if( oc >= 0 )  //есть предыдущий код
			        {
				        codes[st.maxCode].prefix = (short)oc;
				        codes[st.maxCode].sym = codes[s].sym;
				        st.maxCode++;
			        }

                    loops = 2 * codes.Length;
                    while( s >= 0 && loops > 0 )
			        {
                        loops--;
				        to[saved++] = codes[s].sym;
				        s = codes[s].next;
			        }
                    if (loops <= 0)
                    {
                        saved = -2;
                        break;
                    }
                }
		        else
		        {
			        s = oc;
					codes[s].next = -1;

                    loops = 2 * codes.Length;
			        while( codes[s].prefix >= 0 && loops > 0 ) 
			        {
                        loops--;
				        codes[codes[s].prefix].next = (short)s;
				        s = codes[s].prefix;
			        }
                    if (loops <= 0)
                    {
                        saved = -3;
                        break;
                    }

			        codes[st.maxCode].prefix = (short)oc;
			        codes[st.maxCode].sym = codes[s].sym; // s остался от предыдущего цикла (строка с кодом oc)
			        st.maxCode++;
			        int c2 = codes[s].sym;

                    loops = 2 * codes.Length;
			        while( s >= 0 && loops > 0  ) 
			        {
                        loops--;
				        to[saved++] = codes[s].sym;
				        s = codes[s].next;
			        }
                    if (loops <= 0)
                    {
                        saved = -4;
                        break;
                    }
                    
                    to[saved++] = (byte)c2;
		        }
		        oc = c;
	        }
	        return saved;
        }
    }
}
