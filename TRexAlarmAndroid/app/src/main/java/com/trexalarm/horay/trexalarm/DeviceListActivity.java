package com.trexalarm.horay.trexalarm;

/**
 * Created by HoRay on 11/6/2017.
 */

import java.util.ArrayList;
import java.util.Set;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;


public class DeviceListActivity extends Activity {
    // Debugging for LOGCAT
    private static final String TAG = "DeviceListActivity";
    private static final boolean D = true;

    String names;
    ArrayList<String> addresses = new ArrayList<String>();
    String address;

    Button connectButton, rechoose;

    TextView textView1;

    // EXTRA string to send on to mainactivity
    public static String EXTRA_DEVICE_ADDRESS = "device_address";

    // Member fields
    private BluetoothAdapter mBtAdapter;
    private ArrayAdapter<String> mPairedDevicesArrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.device_list);

        connectButton = (Button) findViewById(R.id.connectButton);
        rechoose = (Button) findViewById(R.id.rechoose);

        connectButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Make an intent to start next activity while taking an extra which is the MAC address.
                Intent i = new Intent(DeviceListActivity.this, MainActivity.class);
                i.putExtra("addressList", addresses);
                if(addresses.size() > 0){
                    Toast.makeText(getBaseContext(), "Connecting", Toast.LENGTH_SHORT).show();

                    startActivity(i);
                }
                else{
                    Toast.makeText(getBaseContext(), "None selected", Toast.LENGTH_SHORT).show();
                }

            }
        });
        rechoose.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Make an intent to start next activity while taking an extra which is the MAC address.
                addresses.clear();
                names = "";
                textView1.setText(names);

            }
        });
    }

    @Override
    public void onResume()
    {
        super.onResume();
        //***************
        checkBTState();


        addresses.clear(); //check this later
        names = " ";

        textView1 = (TextView) findViewById(R.id.connecting);
        textView1.setTextSize(40);
        textView1.setText(" ");

        // Initialize array adapter for paired devices
        mPairedDevicesArrayAdapter = new ArrayAdapter<String>(this, R.layout.device_name);

        // Find and set up the ListView for paired devices
        ListView pairedListView = (ListView) findViewById(R.id.paired_devices);
        pairedListView.setAdapter(mPairedDevicesArrayAdapter);
        pairedListView.setOnItemClickListener(mDeviceClickListener);

        // Get the local Bluetooth adapter
        mBtAdapter = BluetoothAdapter.getDefaultAdapter();

        // Get a set of currently paired devices and append to 'pairedDevices'
        Set<BluetoothDevice> pairedDevices = mBtAdapter.getBondedDevices();

        // Add previosuly paired devices to the array
        if (pairedDevices.size() > 0) {
            findViewById(R.id.title_paired_devices).setVisibility(View.VISIBLE);//make title viewable
            for (BluetoothDevice device : pairedDevices) {
                mPairedDevicesArrayAdapter.add(device.getName() + "\n" + device.getAddress());
            }
        } else {
            String noDevices = getResources().getText(R.string.none_paired).toString();
            mPairedDevicesArrayAdapter.add(noDevices);
        }
    }
    //TODO:: This is where you could have multiple bluetooth activities at the same time
    //       Send an array of extras of device actives not just one.
    //       Set text to To Connect.
    //       Allow user to know to "Select all sensors" then a button that says connect
    //       If none selected error out.
    // Set up on-click listener for the list (nicked this - unsure)
    //Make sure it's not reselectable
    private OnItemClickListener mDeviceClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> av, View v, int arg2, long arg3) {

            //textView1.setText("Connecting...");
            // Get the device MAC address, which is the last 17 chars in the View
            names = "";
            String info = ((TextView) v).getText().toString();
            address = info.substring(info.length() - 17);
            Boolean skip = false;
            for(int i = 0; i < addresses.size(); i ++) {
                if(addresses.get(i) == address){
                    skip = true;
                    break;
                }
            }
            if(!skip){
                addresses.add(address);
            }

            for(int i = 0; i < addresses.size(); i ++) {
                names += addresses.get(i) + "\n";
            }
            textView1.setText(names);

        }
    };


    private void checkBTState() {
        // Check device has Bluetooth and that it is turned on
        mBtAdapter=BluetoothAdapter.getDefaultAdapter(); // CHECK THIS OUT THAT IT WORKS!!!
        if(mBtAdapter==null) {
            Toast.makeText(getBaseContext(), "Device does not support Bluetooth", Toast.LENGTH_SHORT).show();
        } else {
            if (mBtAdapter.isEnabled()) {
                Log.d(TAG, "...Bluetooth ON...");
            } else {
                //Prompt user to turn on Bluetooth
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, 1);

            }
        }
    }
}
