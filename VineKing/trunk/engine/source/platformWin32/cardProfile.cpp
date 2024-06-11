#include "console/console.h"
#include "platformWin32/platformWin32.h"

void initDisplayDeviceInfo()
{
   Con::printf( "Reading Display Device information..." );

   U32 i = 0;
   DISPLAY_DEVICE ddData;
   ddData.cb = sizeof( DISPLAY_DEVICE );

   // Search for the primary display adapter, because that is what the rendering
   // context will get created on.
   while( EnumDisplayDevices( NULL, i, &ddData, 0 ) != 0 )
   {
      // If we find the primary display adapter, break out
      if( ddData.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
         break;

      i++;
   }

   Con::printf( "   Primary Display Device Found:" );

   // Ok, now we have the primary display device. Parse the device information.
   char ven[9];
   char dev[9];

   ven[8] = dev[8] = '\0';

   char *pos = dStrstr( (const char*)ddData.DeviceID,(const char *)"VEN_" );

   if( pos )
      dMemcpy( ven, pos, sizeof(char) * 8 );
   else
      dStrcpy( ven, "VEN_0000" );

   Con::printf( "      Vendor Id: %s", ven );

   pos = dStrstr( (const char*)ddData.DeviceID, (const char *)"DEV_" );

   if( pos )
      dMemcpy( dev, pos, sizeof(char) * 8 );
   else
      dStrcpy( dev, "DEV_0000" );

   Con::printf( "      Device Id: %s", dev );

   // We now have the information, set them to console variables so we can parse
   // the file etc in script using getField and so on.
   Con::setVariable( "$PCI_VEN", ven );
   Con::setVariable( "$PCI_DEV", dev );
}

ConsoleFunction( initDisplayDeviceInfo, void, 1, 1, "() Prints display device info to console and sets them to console variables:\n"
				"\tVendor->$PCI_VEN\n"
				"\tDevice_ID->$PCI_DEV\n"
				"@return No Return Value")
{
   initDisplayDeviceInfo();
}