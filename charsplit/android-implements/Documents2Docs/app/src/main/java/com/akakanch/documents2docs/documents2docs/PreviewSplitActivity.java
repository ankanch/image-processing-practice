package com.akakanch.documents2docs.documents2docs;

import android.graphics.Bitmap;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

public class PreviewSplitActivity extends AppCompatActivity {

    private Button btnPrevious;
    private Button btnNext;
    private TextView tvInfo;
    private TextView tvCount;
    private ImageView ivPreview;
    private String curcount_suffix = "/";
    private int cur = 0;

    public static ArrayList<Bitmap> imagelist;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_preview_split);

        btnPrevious = (Button)findViewById(R.id.button_preview_previous);
        btnNext = (Button)findViewById(R.id.button_preview_next);
        tvInfo = (TextView)findViewById(R.id.textView_previewinfor);
        tvCount = (TextView)findViewById(R.id.textView_preview_count);
        ivPreview = (ImageView)findViewById(R.id.imageView_preview_image);
        setTitle("Preview Segments");

        initData();
        setClickListener();
    }

    private void setClickListener(){

        btnPrevious.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cur--;
                ivPreview.setImageBitmap(imagelist.get(cur));
                btnNext.setEnabled(true);
                tvCount.setText( Integer.toString(cur+1) + curcount_suffix );
                if(cur - 1 == -1){
                    btnPrevious.setEnabled(false);
                }
            }
        });

        btnNext.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cur++;
                ivPreview.setImageBitmap(imagelist.get(cur));
                btnPrevious.setEnabled(true);
                tvCount.setText( Integer.toString(cur+1) + curcount_suffix );
                if(cur + 1 == imagelist.size()){
                    btnNext.setEnabled(false);
                }
            }
        });
    }

    private void initData(){
        String info = "Total Segments Count:" + Integer.toString(imagelist.size());
        tvInfo.setText(info);
        curcount_suffix += Integer.toString(imagelist.size());
        tvCount.setText("1" + curcount_suffix);

        // load first picture
        ivPreview.setImageBitmap(imagelist.get(cur));
        btnPrevious.setEnabled(false);
    }
}
