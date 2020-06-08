using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Common
{
    public static class MyConvert
    {
        public static int ByteToInt32( byte[] Data, int offset )
        {
            int acc = offset;
            byte[] tmp = new byte[4];
            for (int i = 0; i < 4; i++)
            {
                tmp[i] = Data[i + offset];
            }

            return BitConverter.ToInt32(tmp, 0);
        }

        public static float ByteToFloat(byte[] Data, int offset)
        {
            int acc = offset;
            byte[] tmp = new byte[4];
            for(int i = 0; i<4; i++)
            {
                tmp[i] = Data[i + offset];
            }

            return BitConverter.ToSingle(tmp, 0);
        }

        public static void Int32ToByte( int Value, byte[] Data, int offset )
        {
            int acc = offset;

            Data[acc++] = (byte)(Value&0xff);
            Data[acc++] = (byte)((Value>>8)&0xff);
            Data[acc++] = (byte)((Value>>16)&0xff);
            Data[acc] = (byte)((Value>>24)&0xff);     
        }
    }
}
