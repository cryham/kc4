#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#ifdef GSM
#include "SoftwareSerial.h"
extern SoftwareSerial serGSM;
#endif


//  Draw GSM
//....................................................................................
void Gui::DrawGSM()
{
	//  title
	DrawTitle("GSM", RGB(17,22,22), &bmpTEST);
	d->setFont(OpenSans10);
	d->setClr(23,27,31);

#ifndef GSM
	d->print("Not available");
	return;
#else

	//-----------------------------------------------------
	int16_t y = yTitle-10;
	d->setCursor(10,y);

	switch (ym2GSM)  // cur cmd text
	{
	case 0:  d->print("AT  OK?");  break;
	case 1:  d->print("AT+CSQ  Signal quality 0-31 best");  break;
	case 2:  d->print("AT+CCID  SIM info");  break;
	case 3:  d->print("AT+CREG?  reg netw?");  break;
	case 4:  d->print("ATI  Get module name, rev");  break;
	case 5:  d->print("AT+COPS?  Check network");  break;
	// AT+COPS=? – Return the list of operators present in the network.*/
	// send sms
	case 6:  d->println("AT+CMGF=1  send SMS mode");  break;  // text mode
	case 7:  d->println("AT+CMGS=\"+CCxxx\"  send sms");  break;
	case 8:  d->print("Hello from KC4  sms text");  break;
	case 9:  d->print("sms send end");  break;
	// recv sms
	case 10:  d->println("AT+CMGF=1  recv SMS mode");  break;
	case 11:  d->println("AT+CNMI=1,2,0,0,0  recv sms");  break;  // Decides how newly arrived SMS messages should be handled
	// call
	case 12:  d->println("ATD+CCxxx  call");  break;
	case 13:  d->println("ATH  hang up");  break;
	case 14:  d->println("ATA  accept incoming call");  break;
	case 15:  d->println("ATE0  echo off");  break;
	// sms store
	case 16:  d->println("AT+CPMS=?  find stores");  break;
	case 17:  d->println("AT+CPMS=\"SM\"  store sms in sim");  break;
	case 18:  d->println("AT+CMGL=\"ALL\"  list all sms");  break;
	case 19:  d->println("AT+CMGR=255  read 1st sms");  break;
	case 20:  d->println("AT+CMGR=0  read 1st sms");  break;
	case 21:  d->println("AT+CMGD=2  delete 2nd sms");  break;
	}

	//  info  ----
	d->setCursor(210, 70);
	d->setClr(29, 30, 31);
	d->print("  y: ");  d->print(ym2GSM);
	d->setFont(0);
	d->setCursor(10, 80);

	d->print(gsmStr.c_str());
#endif
}

//  Keys GSM
//....................................................................................
void Gui::KeysGSM(int sp)
{
	#ifdef GSM
	if (kUp)
	{	ym2GSM  = RangeAdd(ym2GSM, kUp, 0, gsmCmd, 1);  }
	else
	if (kRight)
	{
		gsmStr.clear();
		switch (ym2GSM)
		{
		case 0:  serGSM.write("AT");  break;  // Once the handshake test is successful, it will back to OK
		case 1:  serGSM.println("AT+CSQ");  break;  // Signal quality test, value range is 0-31 , 31 is the best
		case 2:  serGSM.println("AT+CCID");  break;  // Read SIM information to confirm whether the SIM is plugged
		case 3:  serGSM.println("AT+CREG?");  break;  // Check whether it has registered in the network
		case 4:  serGSM.println("ATI");  break;  //  Get the module name and revision
		case 5:  serGSM.println("AT+COPS?");  break;  // Check which network you are connected to
		// AT+COPS=? – Return the list of operators present in the network.*/
		// send sms
		case 6:  serGSM.println("AT+CMGF=1");  break;  //  Configuring TEXT mode
		case 7:  serGSM.println("AT+CMGS=\"+CC123456789\"");   break;  //  country code and phone number to sms
		case 8:  serGSM.print("Hello from KC4");   break;  //  text content
		case 9:  serGSM.write(26);  break;  // sms send end
		// recv sms
		case 10:  serGSM.println("AT+CMGF=1");  break;  //  Configuring TEXT mode
		case 11:  serGSM.println("AT+CNMI=1,2,0,0,0");  break;  //  Decides how newly arrived SMS messages should be handled
		// call
		case 12:  serGSM.println("ATD+CC123456789");   break;  //  country code phone number to dial
		case 13:  serGSM.println("ATH");  break;  //  hang up
		case 14:  serGSM.println("ATA");  break;  //  accepts incoming call.
		// other
		case 15:  serGSM.println("ATE0");  break;  //  echo off
		// sms store
		case 16:  serGSM.println("AT+CPMS=?");  break;  //  find out which storages are supported
		case 17:  serGSM.println("AT+CPMS=\"ME\"");  break;  // set sms store to sim
		case 18:  serGSM.println("AT+CMGL=\"ME\"");  break;  //  list all sms
		case 19:  serGSM.println("AT+CMGR=255");  break;  //  read 1st sms
		case 20:  serGSM.println("AT+CMGR=0");  break;  //  read 1st sms
		case 21:  serGSM.println("AT+CMGD=2");  break;  //  delete 2nd sms
	}	}
	#endif
	if (kAdd || kBckSp)  --mlevel;
	//KeysLoadSave();
}
