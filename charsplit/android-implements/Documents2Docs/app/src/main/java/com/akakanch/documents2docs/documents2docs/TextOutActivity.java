package com.akakanch.documents2docs.documents2docs;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class TextOutActivity extends AppCompatActivity {

    private EditText etOutdata;
    private Button betCopy;

    public static String textout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_text_out);
        etOutdata = (EditText)findViewById(R.id.editText_textout);
        betCopy = (Button)findViewById(R.id.button_textout_copy);
        setTitle("Get Text");

        init();
        setClickListener();
    }

    private void init(){
        etOutdata.setText(textout);
    }

    private void setClickListener(){

        betCopy.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                    ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                    ClipData clip = ClipData.newPlainText("document2docs", textout);
                    clipboard.setPrimaryClip(clip);
                    Snackbar.make(view, "Text has been clipped to the clipboard", Snackbar.LENGTH_LONG).show();
            }
        });

    }
}
