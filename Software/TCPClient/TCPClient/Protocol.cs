using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Common;

namespace TCPClient
{
    public class Protocol
    {
        public const byte SENTINEL1 = 0x81;
        public const byte SENTINEL2 = 0x7F;		

        byte[] aData = new byte[12];

        public float Distance
        {
            get { return MyConvert.ByteToFloat(aData, 0); }
        }

        public float Radian
        {
            get { return MyConvert.ByteToFloat(aData, 4); }
        }

        public bool Valid
        {
            get { return Convert.ToBoolean(aData[8]); }
        }


        public void GetData( byte[] Data )
        {
            aData.CopyTo(Data, 0);
        }

        public void SetData( byte[] Data )
        {
            Data.CopyTo(aData, 0);
        }


#if false
        public Int32 hTotalSize
        {
            get { return MyConvert.ByteToInt32( hData, 1 ); }
            set { MyConvert.Int32ToByte( value, hData, 1 ); }
        }
#endif
    }
}
