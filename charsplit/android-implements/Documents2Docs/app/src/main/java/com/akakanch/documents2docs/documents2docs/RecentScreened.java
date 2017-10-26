package com.akakanch.documents2docs.documents2docs;

import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v4.content.FileProvider;
import android.util.Log;
import android.widget.ImageView;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class RecentScreened extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    static String imagePath = "Documents2Docs";
    static String last_taken_pictures_abpath = "";
    static final int REQUEST_IMAGE_CAPTURE = 1;
    static final int REQUEST_PICK_IMAGE = 2;

    private ImageView ivtest;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_recent_screened);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        FloatingActionButton fab_add  = (FloatingActionButton)findViewById(R.id.fab_open);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG).setAction("Action", null).show();
                dispatchTakePictureIntent();
            }
        });
        fab_add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(Intent.ACTION_PICK);
                intent.setType("image/*");
                startActivityForResult(intent, REQUEST_PICK_IMAGE);
            }
        });

        ivtest = (ImageView)findViewById(R.id.imageView);
        createDirIfNotExists(imagePath);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_recent_screened, menu);
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

    private void dispatchTakePictureIntent() {
        Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
            File photoFile = null;
            try {
                photoFile = createImageFile();
            } catch (Exception ex) {
                Log.v("ErrorSavingFile:", ex.getMessage());
            }
            if (photoFile != null) {
                Uri photoURI = FileProvider.getUriForFile(this,
                        "com.example.android.fileprovider",
                        photoFile);
                takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT, photoURI);
                startActivityForResult(takePictureIntent, REQUEST_IMAGE_CAPTURE);
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if ( resultCode == RESULT_OK) {
            if(requestCode == REQUEST_IMAGE_CAPTURE ) {
                Bitmap imageBitmap = getPicture( last_taken_pictures_abpath );
                imageBitmap = compressAsJPG(imageBitmap);
                imageBitmap = scaleBitmap(imageBitmap,0.4f);
                ivtest.setImageBitmap(imageBitmap);
                ThumbnailActivity.target_image = imageBitmap;
                Intent confirm_screen = new Intent(getApplicationContext(), ThumbnailActivity.class);
                startActivity(confirm_screen);
            }else if( requestCode == REQUEST_PICK_IMAGE ){
                Toast.makeText(this,"open image" + data.getData(), Toast.LENGTH_LONG).show();
                //open image and pass it to the C++ function
                Bitmap bp = getPicture(data.getData());
                bp = compressAsJPG(bp);
                ThumbnailActivity.target_image = bp;
                Intent confirm_screen = new Intent(getApplicationContext(), ThumbnailActivity.class);
                ivtest.setImageBitmap(bp);
                startActivity(confirm_screen);
            }
        }
    }

    //https://stackoverflow.com/questions/25168116/android-how-to-open-image-from-gallery
    //https://stackoverflow.com/questions/8710515/reading-an-image-file-into-bitmap-from-sdcard-why-am-i-getting-a-nullpointerexc
    public  Bitmap getPicture(Uri selectedImage) {
        String[] filePathColumn = { MediaStore.Images.Media.DATA };
        Cursor cursor = getApplicationContext().getContentResolver().query(selectedImage, filePathColumn, null, null, null);
        cursor.moveToFirst();
        int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
        String picturePath = cursor.getString(columnIndex);
        cursor.close();
        Log.v("path=",picturePath);
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        return BitmapFactory.decodeFile(picturePath,options);
    }

    public  Bitmap getPicture(String path) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        return BitmapFactory.decodeFile(path,options);
    }

    public Bitmap compressAsJPG(Bitmap bp){
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        bp.compress(Bitmap.CompressFormat.JPEG, 100, out);
        return  BitmapFactory.decodeStream(new ByteArrayInputStream(out.toByteArray()));
    }

    public Bitmap scaleBitmap(Bitmap srcBitmap,float ratio){
        int srcWidth = srcBitmap.getWidth();
        int srcHeight = srcBitmap.getHeight();
        int dstWidth = (int)(srcWidth*ratio);
        int dstHeight = (int)(srcHeight*ratio);
        return Bitmap.createScaledBitmap(srcBitmap, dstWidth, dstHeight, true);
    }

    public static boolean createDirIfNotExists(String path) {
        boolean ret = true;

        File file = new File(Environment.getExternalStorageDirectory(), path);
        if (!file.exists()) {
            if (!file.mkdirs()) {
                Log.e("CreatingFolder=", "Problem creating Image folder");
                ret = false;
            }
        }
        return ret;
    }

    private File createImageFile() throws IOException {
        // Create an image file name
        String timeStamp = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss").format(new Date());
        String imageFileName = "Documents2Docs_" + timeStamp;
        File storageDir = new File( Environment.getExternalStorageDirectory(), imagePath );
        File image = File.createTempFile(
                imageFileName,  /* prefix */
                ".jpg",         /* suffix */
                storageDir      /* directory */
        );
        last_taken_pictures_abpath = image.getAbsolutePath();
        return image;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    // returnimg: large than 100 means return all splited images, otherwise, return reconized chars;
    public static native String processImageJNI(int[] ipcData,int width,int height,int channels,int returnimg);
}
