package com.pyros2097.remoteMonitor;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.SmsMessage;
import android.widget.Toast;

public class SmsReceiver extends BroadcastReceiver
{
	@Override
	public void onReceive(Context context, Intent intent) 
	{
        //---get the SMS message passed in---
        Bundle bundle = intent.getExtras();        
        SmsMessage[] msgs = null;
        String str = "";            
        if (bundle != null)
        {
            //---retrieve the SMS message received---
            Object[] pdus = (Object[]) bundle.get("pdus");
            msgs = new SmsMessage[pdus.length];            
            for (int i=0; i<msgs.length; i++){
                msgs[i] = SmsMessage.createFromPdu((byte[])pdus[i]);                
                //str += "SMS from " + msgs[i].getOriginatingAddress();                     
                //str += " :";
                str += msgs[i].getMessageBody().toString();
                parseSMS(str);
            }
            //---display the new SMS message---
            Toast.makeText(context, str, Toast.LENGTH_SHORT).show();
        }                 		
	}
	
	public void parseSMS(String msg){
		String fval= "";
		if(msg.contains("Humidity")){
			fval = msg.substring(10, 15);
			SMSTest.setHum(fval);
		}
		if(msg.contains("Temperature")){
			fval = msg.substring(13, 18);
			SMSTest.setTemp(fval);
		}
		if(msg.contains("Automatic Mode")){
			SMSTest.setMode("Mode: AUTO");
		}
		if(msg.contains("Manual Mode")){
			SMSTest.setMode("Mode: MANUAL");
		}
	}
}