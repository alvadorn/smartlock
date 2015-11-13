package com.nikolas.ceunes.ardutooth;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

public class LoginActivity extends AppCompatActivity {

    private Button previous;
    private Button sendRequest;
    private String login;
    private String password;
    private String token;
    private EditText loginInput;
    private EditText passwordInput;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        login = new String();
        password = new String();
        token = new String();
        loginInput = (EditText) findViewById(R.id.editText);
        passwordInput = (EditText) findViewById(R.id.editText2);
        previous = (Button) findViewById(R.id.button6);
        sendRequest = (Button) findViewById(R.id.button7);

        /*
        Loads screen after requesting it from the previous screen.
         */
        previous.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Intent intent = new Intent();
                setResult(RESULT_OK, intent);
                finish();
            }
        });

        /*
        Receives the token data extracted from the previous screen.
         */
        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            this.token = extras.getString("passingToken");
        }

        /*
        Sets the input text listeners.
         */
        loginInput.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                boolean handled = false;
                if(actionId == EditorInfo.IME_ACTION_NEXT) {
                    String inputText = v.getText().toString();
                    Toast.makeText(LoginActivity.this, "Your login is: " + inputText, Toast.LENGTH_SHORT).show();

                    InputMethodManager inputManager = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    inputManager.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(),InputMethodManager.HIDE_NOT_ALWAYS);
                }
                return handled;
            }
        });
        passwordInput.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                boolean handled = false;
                if(actionId == EditorInfo.IME_ACTION_NEXT) {
                    String inputText = v.getText().toString();
                    Toast.makeText(LoginActivity.this, "Your password is: " + inputText, Toast.LENGTH_SHORT).show();

                    InputMethodManager inputManager = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    inputManager.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
                }
                return handled;
            }
        });



    }

    protected void send() {
        JSONObject bj = new JSONObject();
        try {
            bj.put("account", loginInput.getText().toString());
            bj.put("password", passwordInput.getText().toString());
            bj.put("token",token);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_SEND);
        intent.setType("image/*");
        intent.putExtra(Intent.EXTRA_STREAM, bj.toString());
        startActivity(intent);


    }

}
