package com.akakanch.documents2docs.documents2docs;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.AsyncTask;
import android.support.annotation.IntegerRes;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public class ThumbnailActivity extends AppCompatActivity {

    private ImageView iv_thumbnail;
    private TextView tv_test;
    private String result;
    private  ArrayList<Bitmap> p;
    private int pos = 0;
    private Context context;

    public static Bitmap target_image;      // target image to process


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_thumbnail);
        context = this;

        iv_thumbnail = (ImageView) findViewById(R.id.imageView_thumbnail);
        tv_test = (TextView)findViewById(R.id.textView_test);

        // get image data
        iv_thumbnail.setImageBitmap(target_image);
        ((Button)findViewById(R.id.button_previous)).setEnabled(false);
        ((Button)findViewById(R.id.button_next)).setEnabled(false);

        ((Button)findViewById(R.id.button_process)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new getImage().execute(target_image);
                ((Button)findViewById(R.id.button_process)).setEnabled(false);
                ((Button)findViewById(R.id.button_previous)).setEnabled(true);
                ((Button)findViewById(R.id.button_next)).setEnabled(true);

            }
        });

        ((Button)findViewById(R.id.button_next)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(pos < p.size()) {
                    iv_thumbnail.setImageBitmap(p.get(pos));
                    pos++;
                }else{
                    Toast.makeText(getApplicationContext(),"It's been the last picture",Toast.LENGTH_LONG).show();
                }
            }
        });

        ((Button)findViewById(R.id.button_previous)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(pos > 0 ) {
                    iv_thumbnail.setImageBitmap(p.get(pos));
                    pos--;
                }else{
                    Toast.makeText(getApplicationContext(),"It's been the first picture",Toast.LENGTH_LONG).show();
                }
            }
        });

        ((Button)findViewById(R.id.button_gettext)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new processImage().execute(target_image);
            }});
    }

    //获取识别出的字符
    private class processImage extends AsyncTask<Bitmap, Void, String> {

        final private ProgressDialog loading= new ProgressDialog(context);

        @Override
        protected String doInBackground(Bitmap... bitmaps) {
            Bitmap image = bitmaps[0];
            int[] iimg = bmp2ChannelArray(image);
            Log.v("convert2array","2");
            String result =   RecentScreened.processImageJNI(iimg,image.getWidth(),image.getHeight(),3,2);
            Log.v("returned","3");
            return result;
        }


        @Override
        protected void onPostExecute(String s) {
            Log.v("result=",s);
            loading.dismiss();
            result = s;
            tv_test.setText("Recognize Result:" + result);
        }

        @Override
        protected void onPreExecute() {
            loading.setMessage("处理中...");
            loading.show();
            loading.setCancelable(false);
            loading.setCanceledOnTouchOutside(false);
        }
    }

    //获取切割后的图片
    private class getImage extends AsyncTask<Bitmap, Void, String> {

        final private ProgressDialog loading= new ProgressDialog(context);

        @Override
        protected String doInBackground(Bitmap... bitmaps) {
            Bitmap image = bitmaps[0];
            int[] iimg = bmp2ChannelArray(image);
            String result =   RecentScreened.processImageJNI(iimg,image.getWidth(),image.getHeight(),3,500);
            Log.v("result(getImage)=",result);
            return result;
        }


        @Override
        protected void onPostExecute(String s) {
            p = ImageHelper.getImageFromCPP(s);
            iv_thumbnail.setImageBitmap(p.get(0));
            pos = 1;
            tv_test.setText("Image segmentation sum:" + Integer.toString(p.size()));
            loading.dismiss();
        }

        @Override
        protected void onPreExecute() {
            loading.setMessage("处理中...");
            loading.show();
            loading.setCancelable(false);
            loading.setCanceledOnTouchOutside(false);
        }
    }


    // convert bitmap to an byte array
    private int[] bmp2ChannelArray(Bitmap bmp){
        ArrayList<Integer> result = new ArrayList<>();
        //extract
        for(int i=0;i<bmp.getHeight();i++){
            for(int j=0;j<bmp.getWidth();j++){
                //extract R,G,B value from pixels
                int pixel = bmp.getPixel(j,i);
                int red = (pixel >> 16) & 0xff;
                int green = (pixel >>8 ) & 0xff;
                int blue = (pixel) & 0xff;

                result.add(red);
                result.add(green);
                result.add(blue);
            }
        }
        //return int Array
        int[] barray = new int[result.size()];
        for(int i=0;i<result.size();i++){
            barray[i] = result.get(i).intValue();
        }

        return barray;
    }



}
