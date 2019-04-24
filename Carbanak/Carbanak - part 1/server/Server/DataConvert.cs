using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Server
{
	public class DataConvert
	{
		static public int DateTimeToBytes(DateTime dt, byte[] data, int i)
		{
			data[i++] = (byte)(dt.Year & 0xff);
			data[i++] = (byte)(dt.Year >> 8);
			data[i++] = (byte)dt.Month;
			data[i++] = (byte)dt.Day;
			data[i++] = (byte)dt.Hour;
			data[i++] = (byte)dt.Minute;
			data[i++] = (byte)dt.Second;
			return i;
		}

		static public DateTime GetDateTime(byte[] data, int i)
		{
			int year = GetShort(data, i);
			int month = data[i + 2];
			int day = data[i + 3];
			int hour = data[i + 4];
			int minute = data[i + 5];
			int second = data[i + 6];
			return new DateTime(year, month, day, hour, minute, second);
		}

		static public int GetShort(byte[] data, int i)
		{
			if (data.Length - i >= 4)
				return data[i] + data[i + 1] * 256;
			return 0;
		}

		static public int PutShort(int v, byte[] data, int i)
		{
			data[i++] = (byte)(v & 0xff);
			data[i++] = (byte)((v >> 8) & 0xff);
			return i;
		}

		static public uint GetUInt(byte[] data, int i)
		{
			if( data.Length - i >= 4 )
				return (uint)(data[i] | (data[i + 1] << 8) | (data[i + 2] << 16) | (data[i + 3] << 24));
			return 0;
		}

		static public int PutUInt(uint v, byte[] data, int i)
		{
			data[i++] = (byte)(v & 0xff);
			data[i++] = (byte)((v >> 8) & 0xff);
			data[i++] = (byte)((v >> 16) & 0xff);
			data[i++] = (byte)((v >> 24) & 0xff);
			return i;
		}

		static public string GetString(byte[] data, ref int i)
		{
			string ret = string.Empty;
			int size = data[i++];
			if (size > 0 && i + size <= data.Length)
			{
				ret = Encoding.ASCII.GetString(data, i, size);
				i += size;
			}
			return ret;
		}
	}
}
