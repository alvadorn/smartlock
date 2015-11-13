package com.nikolas.ceunes.ardutooth;

import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.InputType;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Set;
import java.util.UUID;

public class PairingActivity extends AppCompatActivity {

    Button pairingButton, turnOnButton, turnOffButton, visibleButton, inputButton;

    private BluetoothAdapter adapter;
    private Set<BluetoothDevice> pairedDevices;
    private BluetoothDevice arduino;
    private BluetoothSocket socket;
    private OutputStream outputStream;
    private InputStream inputStream;
    private String token;
    private EditText loginInput;
    private EditText passwordInput;


    final String PREFS_NAME = "MyPrefsFile";
    SharedPreferences settings;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pairing_screen);

        pairingButton = (Button) findViewById(R.id.button);
        turnOnButton = (Button) findViewById(R.id.button2);
        turnOffButton = (Button) findViewById(R.id.button3);
        visibleButton = (Button) findViewById(R.id.button4);
        inputButton = (Button) findViewById(R.id.button5);
        loginInput = (EditText) findViewById(R.id.editText);
        passwordInput = (EditText) findViewById(R.id.editText2);


        adapter = BluetoothAdapter.getDefaultAdapter();
        settings = getSharedPreferences(PREFS_NAME, 0);

        /*
        If application is run for the first time, generate identification token
         */
        if (settings.getBoolean("my_first_time", true)) {
            String date = DateFormat.getDateTimeInstance().format(new Date());
            md5(date);
            settings.edit().putBoolean("my_first_time", false).commit();
        }

        inputButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder alertDialog = new AlertDialog.Builder(PairingActivity.this);
                alertDialog.setTitle("Enter with your credentials");


                final EditText input = new EditText(PairingActivity.this);
                final EditText pass = new EditText(PairingActivity.this);
                input.setInputType(InputType.TYPE_CLASS_TEXT);
                pass.setInputType(InputType.TYPE_CLASS_TEXT);

                LinearLayout layout =new LinearLayout(getApplicationContext());
                layout.setOrientation(LinearLayout.VERTICAL);
                input.setHint("User");
                pass.setHint("Password");
                layout.addView(input);
                layout.addView(pass);
                alertDialog.setView(layout);



                alertDialog.setPositiveButton("Confirm",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {
                                loginInput = input;
                                passwordInput = pass;
                            }
                        }
                );
                alertDialog.setNegativeButton("Cancel",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.cancel();
                            }
                        });
                alertDialog.show();
            }
        });


    }

    public void turnOn(View v) {
        if (!adapter.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 0);
            Toast.makeText(getApplicationContext(), "Turned on!", Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(getApplicationContext(), "Bluetooth already on", Toast.LENGTH_LONG).show();
        }
    }

    public void turnOff(View v) {
        adapter.disable();
        Toast.makeText(getApplicationContext(), "Turned off!", Toast.LENGTH_LONG).show();
    }

    public void turnVisible(View v) {
        Intent getVisible = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
        startActivityForResult(getVisible, 0);
    }

    public void pairDevice(View v) throws IOException {
        pairedDevices = adapter.getBondedDevices();

        /*
        Possible names for the pairing arduino : HC-05 or HC-06
        Setting the pairing password as 1234
         */
        if (pairedDevices.size() > 0) {
            for (BluetoothDevice b : pairedDevices) {
                if (b.getName().equals("HC-05") || b.getName().equals("HC-06")) {
                    arduino = b;
                    break;
                }
            }
        }
        connectToArduino();
    }

    public void connectToArduino() throws IOException {
        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); //Standard SerialPortService ID
        try {
            socket = arduino.createRfcommSocketToServiceRecord(uuid);
        } catch (IOException e) {
            e.printStackTrace();
        }
        socket.connect();
        outputStream = socket.getOutputStream();
        inputStream = socket.getInputStream();
    }

    public void sendData() throws IOException {
        JSONObject bj = new JSONObject();
        try {
            bj.put("account", loginInput.getText().toString());
            bj.put("password", passwordInput.getText().toString());
            bj.put("token", token);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        outputStream.write(bj.toString().getBytes());
        closeConnection();
    }

    public void closeConnection() throws IOException {
        inputStream.close();
        outputStream.close();
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
