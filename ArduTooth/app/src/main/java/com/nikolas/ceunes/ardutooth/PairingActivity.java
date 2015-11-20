package com.nikolas.ceunes.ardutooth;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.DateFormat;
import java.util.Date;
import java.util.Set;
import java.util.UUID;

public class PairingActivity extends Activity {

    Button pairingButton, turnOnButton, turnOffButton, visibleButton, inputButton, instantButton;

    private BluetoothAdapter adapter;
    private Set<BluetoothDevice> pairedDevices;
    private BluetoothDevice arduino;
    private BluetoothSocket socket;
    private OutputStream outputStream;
    private InputStream inputStream;
    private String token;
    private EditText loginInput;
    private EditText passwordInput;
    private TextView showToken;
    private int databaseCheck;

    SharedPreferences prefs = null;
    DatabaseHelper database;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pairing_screen);

        pairingButton = (Button) findViewById(R.id.button);
        turnOnButton = (Button) findViewById(R.id.button2);
        turnOffButton = (Button) findViewById(R.id.button3);
        visibleButton = (Button) findViewById(R.id.button4);
        inputButton = (Button) findViewById(R.id.button5);
        instantButton = (Button) findViewById(R.id.button6);
        loginInput = (EditText) findViewById(R.id.editText);
        passwordInput = (EditText) findViewById(R.id.editText2);
        showToken = (TextView) findViewById(R.id.textView);
        adapter = BluetoothAdapter.getDefaultAdapter();
        prefs = getSharedPreferences("com.nikolas.ceunes.ardutooth", MODE_PRIVATE);
        token = new String();
        database = new DatabaseHelper(this);

        /*
        Will show a token in the bottom screen TextView if one exists in the database
         */
        Cursor c = database.getAllEntries();
        if(c.moveToFirst()) {
            showToken.setText("\t\t\t\t\t\t YOUR TOKEN" + "\n" + c.getString(c.getColumnIndex("token")));
            databaseCheck = 1;
        }

        /*
        Forces buttons to disappear. They'll be re-enabled when the app
        successfully connects to the arduino, eliminating the possibility of
        a crash if the user tries to authenticate with a non-paired device
         */
        inputButton.setEnabled(false);
        instantButton.setEnabled(false);

        /*
        Input and output stream instantiation for Bluetooth communication
         */
        inputStream = new InputStream() {
            @Override
            public int read() throws IOException {
                return 0;
            }
        };
        outputStream = new OutputStream() {
            @Override
            public void write(int oneByte) throws IOException {

            }
        };

        inputButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                AlertDialog.Builder alertDialog = new AlertDialog.Builder(PairingActivity.this);
                alertDialog.setTitle("Enter with your credentials");


                final EditText input = new EditText(PairingActivity.this);
                final EditText pass = new EditText(PairingActivity.this);
                input.setInputType(InputType.TYPE_CLASS_TEXT);
                pass.setInputType(InputType.TYPE_CLASS_TEXT);

                LinearLayout layout = new LinearLayout(getApplicationContext());
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
                                database.insertData(loginInput.getText().toString(), passwordInput.getText().toString(), token);
                                try {
                                    sendData();
                                } catch (IOException e) {
                                    e.printStackTrace();
                                } catch (JSONException e) {
                                    e.printStackTrace();
                                }
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
        Possible names for the pairing arduino with the name SmartLock
         */
        if (pairedDevices.size() > 0) {
            for (BluetoothDevice b : pairedDevices) {
                if (b.getName().equals("SmartLock")) {
                    arduino = b;
                    Toast.makeText(getApplicationContext(), "Conected to Arduino : " + arduino.getName(), Toast.LENGTH_LONG).show();
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
            socket.connect();
            outputStream = socket.getOutputStream();
            inputStream = socket.getInputStream();
        } catch (IOException e) {
            //e.printStackTrace();
            socket.close();
        }
        /*
        Re-enables sending data buttons. The instant authentication
        button will only be re-enables if the databases exists, therefore
        there is content loaded to be sent instantaneously with the
        mentioned button
         */
        inputButton.setEnabled(true);
        if (databaseCheck == 1) {
            instantButton.setEnabled(true);
        }

    }

    public void sendData() throws IOException, JSONException {
        JSONObject bj = new JSONObject();
        Cursor c = database.getAllEntries();

        try {
            c.moveToFirst();
            if (c == null) {
                database.insertData(loginInput.getText().toString(), passwordInput.getText().toString(), token);
                bj.put("account", loginInput.getText().toString());
                bj.put("password", passwordInput.getText().toString());
                bj.put("token", token);
                Log.d("Mensagem!", "Enviando dados do input!");
                Log.d("Login", loginInput.getText().toString());
                Log.d("Password", passwordInput.getText().toString());
                Log.d("Token", token);
            } else {
                bj.put("account", c.getString(c.getColumnIndex("login")));
                bj.put("password", c.getString(c.getColumnIndex("password")));
                bj.put("token", c.getString(c.getColumnIndex("token")));
                Log.d("Mensagem!", "Enviando do banco de dados!");
                Log.d("Login", c.getString(c.getColumnIndex("login")));
                Log.d("Password", c.getString(c.getColumnIndex("password")));
                Log.d("Token", c.getString(c.getColumnIndex("token")));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        outputStream.write(bj.toString().getBytes());
        Toast.makeText(getApplicationContext(), "Credentials sent sucessfully!", Toast.LENGTH_LONG).show();
        c.close();
        closeConnection();
    }

    public void instantAuth(View v) throws IOException, JSONException {
        JSONObject bj = new JSONObject();
        Cursor c = database.getAllEntries();

        try {
            c.moveToFirst();
            if (c == null) {
                database.insertData(loginInput.getText().toString(), passwordInput.getText().toString(), token);
                bj.put("account", loginInput.getText().toString());
                bj.put("password", passwordInput.getText().toString());
                bj.put("token", token);
                Log.d("Mensagem!", "Enviando dados do input!");
                Log.d("Login", loginInput.getText().toString());
                Log.d("Password", passwordInput.getText().toString());
                Log.d("Token", token);
            } else {
                bj.put("account", c.getString(c.getColumnIndex("login")));
                bj.put("password", c.getString(c.getColumnIndex("password")));
                bj.put("token", c.getString(c.getColumnIndex("token")));
                Log.d("Mensagem!", "Enviando do banco de dados!");
                Log.d("Login", c.getString(c.getColumnIndex("login")));
                Log.d("Password", c.getString(c.getColumnIndex("password")));
                Log.d("Token", c.getString(c.getColumnIndex("token")));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        outputStream.write(bj.toString().getBytes());
        Toast.makeText(getApplicationContext(), "Credentials sent sucessfully!", Toast.LENGTH_LONG).show();
        c.close();
        closeConnection();
    }

    public void closeConnection() throws IOException {
        inputStream.close();
        outputStream.close();
        //socket.close();
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
    protected void onResume() {
        super.onResume();

        if (prefs.getBoolean("firstrun", true)) {
            String date = DateFormat.getDateTimeInstance().format(new Date());
            md5(date);
            prefs.edit().putBoolean("firstrun", false).commit();
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
