package com.pyros2097.remoteMonitor;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class SMSTest extends Activity 
{
	Button btn1;
	Button btn2;
	Button btn3;
	Button btn4;
	ToggleButton btn5;
	Button btn6;
	static TextView txt4;
	static TextView txt5;
	static TextView txt8;
	EditText txtPhoneNo;
	EditText etxt3;
	String phoneNo;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);        
        btn1 = (Button) findViewById(R.id.button1);
        btn2 = (Button) findViewById(R.id.button2);
        btn3 = (Button) findViewById(R.id.button3);
        btn4 = (Button) findViewById(R.id.button4);
        btn5 = (ToggleButton) findViewById(R.id.toggleButton1);
        btn6 = (Button) findViewById(R.id.button5);
        txtPhoneNo = (EditText) findViewById(R.id.txtPhoneNo);
        etxt3 = (EditText) findViewById(R.id.editText3);
        txt4 = (TextView) findViewById(R.id.textView4);
        txt5 = (TextView) findViewById(R.id.textView5);
        txt8 = (TextView) findViewById(R.id.textView8);
        
        /*
        Intent sendIntent = new Intent(Intent.ACTION_VIEW);
        sendIntent.putExtra("sms_body", "Content of the SMS goes here..."); 
        sendIntent.setType("vnd.android-dir/mms-sms");
        startActivity(sendIntent);
        */
                
        btn1.setOnClickListener(new View.OnClickListener() 
        {
            public void onClick(View v) 
            {            	
            	phoneNo = txtPhoneNo.getText().toString();              	
            	sendSMS(phoneNo, "HUM");                
            }
        });
        btn2.setOnClickListener(new View.OnClickListener() 
        {
            public void onClick(View v) 
            {            	
            	phoneNo = txtPhoneNo.getText().toString();              	
            	sendSMS(phoneNo, "TEMP");                
            }
        });
        btn3.setOnClickListener(new View.OnClickListener() 
        {
            public void onClick(View v) 
            {            	
            	phoneNo = txtPhoneNo.getText().toString();              	
            	sendSMS(phoneNo, "MAN");                
            }
        });
        btn4.setOnClickListener(new View.OnClickListener() 
        {
            public void onClick(View v) 
            {            	
            	phoneNo = txtPhoneNo.getText().toString();              	
            	sendSMS(phoneNo, "AUTO");                
            }
        });
        btn5.setOnClickListener(new View.OnClickListener() 
        {
            public void onClick(View v) 
            {            	
            	phoneNo = txtPhoneNo.getText().toString();
            	if(btn5.isChecked())
            		sendSMS(phoneNo, "ON");
            	else
            		sendSMS(phoneNo, "OFF");
            }
        });
        btn6.setOnClickListener(new View.OnClickListener() 
        {
            public void onClick(View v) 
            {            	
            	phoneNo = txtPhoneNo.getText().toString();
            	if(etxt3.getText().length()>=2)
            		sendSMS(phoneNo, "SET "+etxt3.getText());
            	else
            		Toast.makeText(getBaseContext(), "Please enter Setpoint greater than 30", Toast.LENGTH_SHORT).show();
            }
        });     
    }
    
    //---sends a SMS message to another device---
    private void sendSMS(String phoneNumber, String message)
    {      
    	/*
        PendingIntent pi = PendingIntent.getActivity(this, 0,
                new Intent(this, test.class), 0);                
            SmsManager sms = SmsManager.getDefault();
            sms.sendTextMessage(phoneNumber, null, message, pi, null);        
        */
    	if (phoneNo.length()<9){
    		Toast.makeText(getBaseContext(), 
            "Please enter mobile number", 
            Toast.LENGTH_SHORT).show();
    		return;
    	}
    	
    	String SENT = "SMS_SENT";
    	String DELIVERED = "SMS_DELIVERED";
    	
        PendingIntent sentPI = PendingIntent.getBroadcast(this, 0,
            new Intent(SENT), 0);
        
        PendingIntent deliveredPI = PendingIntent.getBroadcast(this, 0,
            new Intent(DELIVERED), 0);
    	
        //---when the SMS has been sent---
        registerReceiver(new BroadcastReceiver(){
			@Override
			public void onReceive(Context arg0, Intent arg1) {
				switch (getResultCode())
				{
				    case Activity.RESULT_OK:
					    Toast.makeText(getBaseContext(), "SMS sent", 
					    		Toast.LENGTH_SHORT).show();
					    break;
				    case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
					    Toast.makeText(getBaseContext(), "Generic failure", 
					    		Toast.LENGTH_SHORT).show();
					    break;
				    case SmsManager.RESULT_ERROR_NO_SERVICE:
					    Toast.makeText(getBaseContext(), "No service", 
					    		Toast.LENGTH_SHORT).show();
					    break;
				    case SmsManager.RESULT_ERROR_NULL_PDU:
					    Toast.makeText(getBaseContext(), "Null PDU", 
					    		Toast.LENGTH_SHORT).show();
					    break;
				    case SmsManager.RESULT_ERROR_RADIO_OFF:
					    Toast.makeText(getBaseContext(), "Radio off", 
					    		Toast.LENGTH_SHORT).show();
					    break;
				}
			}
        }, new IntentFilter(SENT));
        
        //---when the SMS has been delivered---
        registerReceiver(new BroadcastReceiver(){
			@Override
			public void onReceive(Context arg0, Intent arg1) {
				switch (getResultCode())
				{
				    case Activity.RESULT_OK:
					    Toast.makeText(getBaseContext(), "SMS delivered", 
					    		Toast.LENGTH_SHORT).show();
					    break;
				    case Activity.RESULT_CANCELED:
					    Toast.makeText(getBaseContext(), "SMS not delivered", 
					    		Toast.LENGTH_SHORT).show();
					    break;					    
				}
			}
        }, new IntentFilter(DELIVERED));        
    	
        SmsManager sms = SmsManager.getDefault();
        sms.sendTextMessage(phoneNumber, null, message, sentPI, deliveredPI);               
    }
 
    public static void setTemp(String temp){
    	txt4.setText(temp);
    }
    
    public static void setHum(String hum){
    	txt5.setText(hum);
    }
    
    public static void setMode(String mode){
    	txt8.setText(mode);
    }
}