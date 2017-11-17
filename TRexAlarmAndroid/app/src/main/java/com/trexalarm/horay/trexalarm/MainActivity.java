package com.trexalarm.horay.trexalarm;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.UUID;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.InputType;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

    Button btnOn, btnOff, rename, changePin, clearLog;
    TextView txtArduino, txtString, txtStringLength, received, status, nameFromDevice, log;
    Handler bluetoothIn;

    final int handlerState = 0;                        //used to identify handler message
    private BluetoothAdapter btAdapter = null;
    //private BluetoothSocket btSocket = null;

    ArrayList<String> addresses = new ArrayList<>();
    ArrayList<BluetoothSocket> btSockets = new ArrayList<>();


    //private ConnectedThread mConnectedThread;
    private ArrayList<ConnectedThread> connectedThreadList = new ArrayList<>();
    private ArrayList<String> befEndLineList = new ArrayList<>();

    // SPP UUID service - this should work for most devices
    UUID BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    //TODO::Change to array of MAC addresses
    // String for MAC address
    private static String pin, pinConfirm;
    String logStr = "";
    //private static String address;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        //Link the buttons and textViews to respective views
        btnOn = (Button) findViewById(R.id.buttonOn);
        btnOff = (Button) findViewById(R.id.buttonOff);
        rename = (Button) findViewById(R.id.rename);
        changePin = (Button) findViewById(R.id.changePin);
        clearLog = (Button) findViewById(R.id.clearLog);

        nameFromDevice = (TextView) findViewById(R.id.nameFromDevice);
        txtString = (TextView) findViewById(R.id.txtString);
        txtStringLength = (TextView) findViewById(R.id.testView1);
        status = (TextView) findViewById(R.id.status);
        nameFromDevice = (TextView) findViewById(R.id.nameFromDevice);
        received = (TextView) findViewById(R.id.received);
        log = (TextView) findViewById(R.id.log);

        bluetoothIn = new Handler() {
            public void handleMessage(android.os.Message msg) {
            if (msg.what == handlerState) {
                String readMessage = (String) msg.obj;
                //will keep combining the chars until end-of-line
                int threadNum = Integer.valueOf(readMessage.substring(0, 1));
                readMessage = readMessage.substring(1, readMessage.length());
                if( befEndLineList.size() < threadNum + 1){
                    befEndLineList.add(readMessage);
                }
                else {
                    befEndLineList.set(threadNum, befEndLineList.get(threadNum) + readMessage);
                }
                // determine the end-of-line

                for (int i = 0; i < befEndLineList.size(); i++) {
                    String befEndLine = befEndLineList.get(i);

                    int endOfLineIndex = befEndLine.indexOf("\n");
                    int endOfCommand = befEndLine.indexOf("\r");

                    if (endOfLineIndex != -1) { //check if character is there if not skip

                        String dataInPrint = befEndLine.substring(0, endOfLineIndex);    // extract string
                        String str, curStr;

                        Toast.makeText(getBaseContext(), dataInPrint, Toast.LENGTH_LONG).show();
                        received.setText(dataInPrint);

                        if (endOfCommand != -1) {
                            if (endOfCommand >= 10) {
                                str = dataInPrint.substring(0, 10);

                                if (str.equals("disconnect")) {

                                    connectedThreadList.get(i).interrupt();
                                    try {
                                        btSockets.get(i).close();
                                    } catch (IOException e) {
                                        e.printStackTrace();
                                    }

                                }
                            }
                        }


                        if (endOfLineIndex >= 6) {
                            str = dataInPrint.substring(0, 6);
                            if (str.equals("status")) {
                                curStr = dataInPrint.substring(6, endOfLineIndex);

                                status.setText(curStr);
                            }
                        }
                    /*if(endOfLineIndex >= 4 ){
                        str = dataInPrint.substring(0, 4);

                        if(str.equals("name")){
                            curStr = dataInPrint.substring(4, endOfLineIndex);
                            nameFromDevice.setText(curStr);
                        }
                    }*/
                        if (dataInPrint.equals("Alarm")) {
                            Intent buzz = new Intent(MainActivity.this, backgroundService.class);
                            MainActivity.this.startService(buzz);
                            //String name = btAdapter.getName();
                            String name = getBluetoothName(btSockets.get(i));
                            Toast.makeText(getBaseContext(), name, Toast.LENGTH_LONG).show();
                            nameFromDevice.setText(name);

                        }
                        logStr += befEndLine + "\n";
                        //Clear the data held in string
                        befEndLineList.set(i, "");


                    }
                }
            }

            }
        };

        btAdapter = BluetoothAdapter.getDefaultAdapter();  // get Bluetooth adapter
        checkBTState();

        btnOff.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                //writeAll(connectedThreadList, "0");
                connectedThreadList.get(1).write("1");

                //Toast.makeText(getBaseContext(), "Turn off sensor", Toast.LENGTH_SHORT).show();
                //String name = btAdapter.getName();
                //String name = getBluetoothName(btSocket);
                //Toast.makeText(getBaseContext(), name, Toast.LENGTH_LONG).show();
                //nameFromDevice.setText(name);
            }
        });

        btnOn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                //writeAll(connectedThreadList, "1");
                connectedThreadList.get(0).write("1");

                //mConnectedThread.write("1");    // Send "1" via Bluetooth
                Toast.makeText(getBaseContext(), "Turn on sensors", Toast.LENGTH_SHORT).show();
            }
        });

        rename.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                final AlertDialog builder = new AlertDialog.Builder(MainActivity.this)
                        .setView(v)
                        .setTitle("Please enter a new name. Warning: The sensor will disconnect.'\\n' Please reconnect after 2 seconds.")
                        .setPositiveButton(android.R.string.ok, null) //Set to null. We override the onclick
                        .setNegativeButton(android.R.string.cancel, null)
                        .create();
                builder.setContentView(R.layout.dialog_rename); //Check this


                // Set up the input
                final EditText input = new EditText(MainActivity.this);
                final EditText btName = (EditText) findViewById(R.id.btName);

                builder.setView(input);

                builder.setOnShowListener(new DialogInterface.OnShowListener() {

                    @Override
                    public void onShow(final DialogInterface dialog) {

                        Button button = ((AlertDialog) dialog).getButton(AlertDialog.BUTTON_POSITIVE);
                        button.setOnClickListener(new View.OnClickListener() {

                            @Override
                            public void onClick(View view) {
                                String btNewNameStr = input.getText().toString();
                                String btNameStr = btName.getText().toString();
                                if(btNewNameStr.length() <= 0){
                                    Toast.makeText(getBaseContext(), "Please enter a new name", Toast.LENGTH_LONG).show();
                                }
                                if(btNameStr.length() <= 0){
                                    Toast.makeText(getBaseContext(), "Please enter a name", Toast.LENGTH_LONG).show();

                                }
                                else{
                                    connectedThreadList.get(Integer.valueOf(btNameStr)).write("4");
                                    try {
                                        Thread.sleep(500);
                                    } catch (InterruptedException e) {
                                        // TODO Auto-generated catch block
                                        e.printStackTrace();
                                    }
                                    connectedThreadList.get(Integer.valueOf(btNameStr)).write(btNewNameStr);
                                    connectedThreadList.get(Integer.valueOf(btNameStr)).write("\n");
                                    dialog.dismiss();

                                }

                                //Dismiss once everything is OK.
                            }
                        });
                        Button button2 = ((AlertDialog) dialog).getButton(AlertDialog.BUTTON_NEGATIVE);
                        button2.setOnClickListener(new View.OnClickListener() {

                            @Override
                            public void onClick(View view) {

                                //Dismiss once everything is OK.
                                dialog.dismiss();
                            }
                        });
                    }
                });
                builder.show();
            }
        });

        changePin.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                //TODO::Change the layout of this AlertDialog builder
                /*final AlertDialog builder = new AlertDialog.Builder(MainActivity.this)
                        .setView(v)
                        .setTitle("Please enter a new Pin. Warning: The sensor will disconnect.'\\n' Please reconnect after 2 seconds.")
                        .setPositiveButton(android.R.string.ok, null) //Set to null. We override the onclick
                        .setNegativeButton(android.R.string.cancel, null)
                        .create();

                // Set up the input
                final EditText input = new EditText(MainActivity.this);
                input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
                builder.setView(input);

                final TextView retype = new TextView(MainActivity.this);
                retype.setText("Please retype the pin");
                builder.setView(retype);

                final EditText input2 = new EditText(MainActivity.this);
                input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
                builder.setView(input2);

                builder.setOnShowListener(new DialogInterface.OnShowListener() {

                    @Override
                    public void onShow(final DialogInterface dialog) {

                        Button button = ((AlertDialog) dialog).getButton(AlertDialog.BUTTON_POSITIVE);
                        button.setOnClickListener(new View.OnClickListener() {

                            @Override
                            public void onClick(View view) {
                                pin = input.getText().toString();
                                pinConfirm = input2.getText().toString();
                                if(pin.length() <= 0){
                                    Toast.makeText(getBaseContext(), "Please enter a pin", Toast.LENGTH_LONG).show();
                                }
                                else if(!pin.equals(pinConfirm)){
                                    Toast.makeText(getBaseContext(), "Pin do not match", Toast.LENGTH_LONG).show();
                                }
                                else{
                                    mConnectedThread.write("5");
                                    try {
                                        Thread.sleep(500);
                                    } catch (InterruptedException e) {
                                        // TODO Auto-generated catch block
                                        e.printStackTrace();
                                    }
                                    mConnectedThread.write(pin);
                                    mConnectedThread.write("\n");
                                    dialog.dismiss();
                                }
                                //Dismiss once everything is OK.
                            }
                        });
                        Button button2 = ((AlertDialog) dialog).getButton(AlertDialog.BUTTON_NEGATIVE);
                        button2.setOnClickListener(new View.OnClickListener() {

                            @Override
                            public void onClick(View view) {

                                //Dismiss once everything is OK.
                                dialog.dismiss();
                            }
                        });
                    }
                });
                builder.show();*/
            }
        });

        clearLog.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                logStr = "";
            }
        });
    }
    private BluetoothSocket createBluetoothSocket(BluetoothDevice device, int i, ArrayList<String> addresses) throws IOException {

        //String temp = "00001101-0000-1000-8000-" + addresses.get(i).replace(":", "");
        //BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-" + addresses.get(i).replace(":", ""));
        BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
        //Toast.makeText(getBaseContext(), temp, Toast.LENGTH_LONG).show();
        //Toast.makeText(getBaseContext(), /*device.getName() + */device.getUuids()[0].getUuid().toString(), Toast.LENGTH_LONG).show();

        return  device.createRfcommSocketToServiceRecord(BTMODULEUUID);
        //creates secure outgoing connection with BT device using UUID
    }

    @Override
    public void onResume() {
        super.onResume();

        //Get MAC address from DeviceListActivity via intent
        Intent intent = getIntent();

        //Get the MAC address from the DeviceListActivity via EXTRA
        addresses = (ArrayList<String>)intent.getSerializableExtra("addressList");


        //create device and set the MAC address
        createDeviceandSetMac(addresses);

    }

    //we want to leave btSockets open when leaving activity because of background checks.
    @Override
    public void onPause()
    {
        super.onPause();
            //Don't leave Bluetooth sockets open when leaving activity
            disconnectAll(btSockets);
    }
    @Override
    public void onDestroy()
    {
        super.onDestroy();
        disconnectAll(btSockets);

        Intent intent = new Intent(MainActivity.this, backgroundService.class);
        MainActivity.this.stopService(intent);

    }

    //Checks that the Android device Bluetooth is available and prompts to be turned on if off
    private void checkBTState() {

        if(btAdapter==null) {
            Toast.makeText(getBaseContext(), "Device does not support bluetooth", Toast.LENGTH_LONG).show();
        } else {
            if (btAdapter.isEnabled()) {
            } else {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, 1);
            }
        }
    }


    //create new class for connect thread
    private class ConnectedThread extends Thread {
        private InputStream mmInStream = null;
        private OutputStream mmOutStream = null;
        private InputStream mmInStream1 = null;
        private OutputStream mmOutStream1 = null;
        private int threadNum;

        //creation of the connect thread
        public ConnectedThread(BluetoothSocket socket, int threadNum) {
            InputStream tmpIn = null;
            OutputStream tmpOut = null;
            this.threadNum = threadNum;

            try {
                //Create I/O streams for connection
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) { }
            if(threadNum == 0) {
                mmInStream = tmpIn;
                mmOutStream = tmpOut;
            }
            else if(threadNum == 1){
                mmInStream1 = tmpIn;
                mmOutStream1 = tmpOut;
            }
        }

        public void run() {
           byte[] buffer = new byte[512];
            int bytes;

            // Keep looping to listen for received messages
            while (true) {
                try {
                    if(threadNum == 0){
                        bytes = mmInStream.available();
                        while( bytes> 0 ){
                            bytes = mmInStream.read(buffer);            //read bytes from input buffer
                            String readMessage = new String(buffer, 0, bytes);
                            String readWithThreadNum = Integer.toString(threadNum) + readMessage;
                            // Send the obtained bytes to the UI Activity via handler

                            bluetoothIn.obtainMessage(handlerState, bytes, -1, readWithThreadNum).sendToTarget();
                        }
                    }
                    else{
                        bytes = mmInStream1.available();
                        while( bytes> 0 ){
                            bytes = mmInStream1.read(buffer);            //read bytes from input buffer
                            String readMessage = new String(buffer, 0, bytes);
                            String readWithThreadNum = Integer.toString(threadNum) + readMessage;
                            // Send the obtained bytes to the UI Activity via handler

                            bluetoothIn.obtainMessage(handlerState, bytes, -1, readWithThreadNum).sendToTarget();
                        }
                    }


                } catch (IOException e) {
                    break;
                }
            }

        }
        //write method
        public void write(String input) {
            byte[] msgBuffer = input.getBytes();           //converts entered String into bytes
            if( threadNum == 0 ){
                try {
                    mmOutStream.write(msgBuffer);                //write bytes over BT connection via outstream
                } catch (IOException e) {
                    Toast.makeText(getBaseContext(), "Failed to connect", Toast.LENGTH_LONG).show();

                    //if you cannot write, close the application
                    finish();

                }
            }
            else{
                try {
                    mmOutStream1.write(msgBuffer);                //write bytes over BT connection via outstream
                } catch (IOException e) {
                    Toast.makeText(getBaseContext(), "Failed to connect", Toast.LENGTH_LONG).show();

                    //if you cannot write, close the application
                    finish();

                }
            }

        }
    }

    public String getBluetoothName(BluetoothSocket btSocket){

        BluetoothDevice connectedDevice = btSocket.getRemoteDevice();
        String name = connectedDevice.getName();
        if(name == null){
            Toast.makeText(getBaseContext(), "Name is NULL!", Toast.LENGTH_LONG).show();
            name = connectedDevice.getAddress();
        }
        return name;
    }

    public void createDeviceandSetMac(ArrayList<String> addresses){
        btSockets.clear();

        for( int i = 0; i < addresses.size(); i++)
        {
            String address = addresses.get(i);

            BluetoothDevice device = btAdapter.getRemoteDevice(address);


            try {
                btSockets.add(createBluetoothSocket(device, i, addresses));


            } catch (IOException e) {
                Toast.makeText(getBaseContext(), "Socket creation failed", Toast.LENGTH_LONG).show();
            }
            // Establish the Bluetooth socket connection.
            //Toast.makeText(getBaseContext(), Integer.valueOf(btSockets.size()), Toast.LENGTH_LONG).show();

            connectAll(btSockets);
        }

    }
    public void connectAll(ArrayList<BluetoothSocket> btSockets){
        connectedThreadList.clear();

        for(int i = 0; i < btSockets.size(); i++){
            try {
                btSockets.get(i).connect();
            } catch (IOException e) {
                try {
                    btSockets.get(i).close();
                } catch (IOException e2) {
                    //insert code to deal with this
                }
            }
            connectedThreadList.add(new ConnectedThread(btSockets.get(i), i));
            connectedThreadList.get(i).start();

            //I send a character when resuming.beginning transmission to check device is connected
            //If it is not an exception will be thrown in the write method and finish() will be called
            //connectedThreadList.get(i).write("x");
        }

    }
    public void disconnectAll(ArrayList<BluetoothSocket> btSockets){
        for(int i = 0; i < btSockets.size(); i++){
                try {
                    btSockets.get(i).close();
                } catch (IOException e2) {
                    //insert code to deal with this
                }
        }
    }
    public void writeAll(ArrayList<ConnectedThread> connectedThreadList, String msg){
        for(int i = 0; i < connectedThreadList.size(); i++){

                connectedThreadList.get(i).write(msg);

        }
    }
}
