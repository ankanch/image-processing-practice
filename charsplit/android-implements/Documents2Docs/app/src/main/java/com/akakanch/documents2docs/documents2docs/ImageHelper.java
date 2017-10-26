package com.akakanch.documents2docs.documents2docs;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.util.Log;

import java.io.IOException;
import java.lang.reflect.Array;
import java.util.ArrayList;

/**
 * Created by kanch on 10/10/2017.
 */

public class ImageHelper {


    public ImageHelper(){

    }

    public static Bitmap pixelArray2GraysacleBMP(ImageData im){
        Bitmap image = Bitmap.createBitmap(im.width,im.height,Bitmap.Config.ARGB_8888);
        for(int i=0;i<im.height;i++){
            for(int j=0;j<im.width;j++){
                int value = im.imagepixel[i][j];
                if(value == 1){
                    image.setPixel(j,i,Color.rgb(0,0,0) );
                }else{
                    image.setPixel(j,i,Color.rgb(255,255,255) );
                }
            }
        }
        return image;
    }

    public static Bitmap array1DToBMP(int[] arr,int width,int height,int channel){
        Bitmap image = Bitmap.createBitmap(width,height,Bitmap.Config.ARGB_8888);
        int sum = width*height*channel;
        int col=0,row=0,pixelcount=0;
        for(int i=0;i<sum;i+=3){
            if( pixelcount%width == 0 && pixelcount != 0 ){
                row++;
                col=0;
            }
            image.setPixel(col,row,Color.rgb(arr[i],arr[i+1],arr[i+2]));
            col++;
            pixelcount++;
        }
        return image;
    }

    public static ImageData strinfy2PixelArray(String data){
        String[] spdata = data.split("@");
        //spdata : 0:image , 1:width , 2:height
        int width = Integer.parseInt(spdata[1]);
        int height = Integer.parseInt(spdata[2]);
        int[][] image = new int[ height ][ width ];
        String[] scpixel = spdata[0].replace("[","").replace("]","").split(",");

        /*/
        Log.i("imgdata=",data);
        Log.i("a-length=",Integer.toString(image.length * image[0].length));
        Log.i("raw string-length=",Integer.toString(scpixel.length));
        Log.i("image-size=",Integer.toString(height + "x" + Integer.toString(width));
        /*/
        //start restore image matrix
        for(int i=0;i<height;i++){
            for(int j=0;j<width;j++){
                image[i][j] = Integer.parseInt( scpixel[ i*width + j ] );
            }
        }
        ImageData im= new ImageData();
        im.pixelsource = spdata[0];
        im.channel = 1;
        im.width = width;
        im.height = height;
        im.imagepixel = image;

        return im;
    }

    public static ArrayList<Bitmap> getImageFromCPP(String raw){
        ArrayList<Bitmap> result = new ArrayList<>();
        String[] imgstrlist = raw.split("<!>");
        Log.v("splited length=",Integer.toString(imgstrlist.length) + "\tOFR=703");
        for(String imagestr : imgstrlist){
            ImageData id = strinfy2PixelArray(imagestr);
            Bitmap bp = pixelArray2GraysacleBMP(id);
            //Log.i("image-size=",Integer.toString(bp.getWidth()) + "x" + Integer.toString(bp.getHeight()));
            result.add(bp);
        }
        return result;
    }

    public static void saveImage2Storage(ArrayList<Bitmap> bplist) throws IOException{

    }

}
