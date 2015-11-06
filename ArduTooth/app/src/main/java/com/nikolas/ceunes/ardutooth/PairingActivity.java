package com.nikolas.ceunes.ardutooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Set;

public class PairingActivity extends AppCompatActivity {

    Button pairingButton, turnOnButton, turnOffButton, listDevicesButton, nextButton;

    private BluetoothAdapter adapter;
    private Set<BluetoothDevice> pairedDevices;
    public ListView listView;
    private String token;

    final String PREFS_NAME = "MyPrefsFile";
    SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pairing_screen);

        pairingButton = (Button)findViewById(R.id.button);
        turnOnButton = (Button)findViewById(R.id.button2);
        turnOffButton = (Button)findViewById(R.id.button3);
        listDevicesButton = (Button)findViewById(R.id.button4);
        nextButton = (Button)findViewById(R.id.button5);

        adapter = BluetoothAdapter.getDefaultAdapter();
        listView = (ListView)findViewById(R.id.listView);

        /*
        If application is run for the first time, generate identification token
         */
        if(settings.getBoolean("my_first_time", true)) {
            String date = DateFormat.getDateTimeInstance().format(new Date());
            md5(date);
            settings.edit().putBoolean("my_first_time", false).commit();
        }

        nextButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Intent myIntent = new Intent(view.getContext(), LoginActivity.class);
                myIntent.putExtra("passingToken", token);
                startActivityForResult(myIntent, 0);
            }

        });

    }

    public void turnOn(View v) {
        if(!adapter.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn,0);
            Toast.makeText(getApplicationContext(),"Turned on!",Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(getApplicationContext(),"Bluetooth already on", Toast.LENGTH_LONG).show();
        }
    }

    public void turnOff(View v) {
        adapter.disable();
        Toast.makeText(getApplicationContext(),"Turned off!" ,Toast.LENGTH_LONG).show();
    }

    public void turnVisible(View v) {
        Intent getVisible = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
        startActivityForResult(getVisible, 0);
    }

    public void listDevice() {
        pairedDevices = adapter.getBondedDevices();
        ArrayList display = new ArrayList();

        for(BluetoothDevice b: pairedDevices) {
            display.add(adapter.getName());
        }
        Toast.makeText(getApplicationContext(),"Paired Devices:",Toast.LENGTH_SHORT).show();

        final ArrayAdapter adapter = new ArrayAdapter(this,android.R.layout.simple_list_item_1, display);
        listView.setAdapter(adapter);

    }

    public final void md5(final String s) {
        final String MD5 = "MD5";
        try {
            // Create MD5 Hash
            MessageDigest digest = java.security.MessageDigest
                    .getInstance(MD5);
            digest.update(s.getBytes());
            byte messageDigest[] = digest.digest();

            // Create Hex String
            StringBuilder hexString = new StringBuilder();
            for (byte aMessageDigest : messageDigest) {
                String h = Integer.toHexString(0xFF & aMessageDigest);
                while (h.length() < 2)
                    h = "0" + h;
                hexString.append(h);
            }
            this.token = hexString.toString();

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_pairing_screen, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
