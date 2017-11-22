package com.akakanch.documents2docs.documents2docs;

import android.Manifest;
import android.app.ProgressDialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.support.annotation.NonNull;
import android.support.design.widget.Snackbar;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.FileProvider;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.LayoutInflater;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ListView;
import android.widget.PopupMenu;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;

public class RecentScreened extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    static String imagePath = "Documents2Docs";
    static String last_taken_pictures_abpath = "";
    static String last_taken_pictures_name = "";
    static final int REQUEST_IMAGE_CAPTURE = 1;
    static final int REQUEST_PICK_IMAGE = 2;

    // configs
    private WriteConfig Config;
    private float compress_rate = 0.2f;

    // data
    private ListView lvrecent;
    private ArrayList<RecentItem> al_recent = new ArrayList<RecentItem>();
    private ArrayAdapter<RecentItem> aa_recent;
    private DatabaseHelper dbhelper;
    private SQLiteDatabase db ;
    private Context CurActivitycontext;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_recent_screened);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        CurActivitycontext = this;
        dbhelper = new DatabaseHelper(getApplicationContext());
        lvrecent = (ListView)findViewById(R.id.listview_recent);
        checkAndRequestPermission();
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        FloatingActionButton fab_add  = (FloatingActionButton)findViewById(R.id.fab_open);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
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

        createDirIfNotExists(imagePath);
        // connect to database
        db = dbhelper.getWritableDatabase();
        new LoadAllRecentInBackground().execute(db);
        setListenerOnListView(lvrecent);
        Config = new WriteConfig(getApplicationContext());
        String compressrate = Config.read("COMPRESSRATE");
        if(compressrate.length() != 0){
            compress_rate = Float.parseFloat(compressrate);
        }else{
            Config.save("COMPRESSRATE",String.valueOf(compressrate));
        }
        Log.v("Compress_rate=", String.valueOf(compress_rate));
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
            LayoutInflater li = LayoutInflater.from(CurActivitycontext);
            final View settingsDlgView = li.inflate(R.layout.settings_layout, null);
            final AlertDialog.Builder builder = new AlertDialog.Builder(CurActivitycontext);
            builder.setView(settingsDlgView);
            final AlertDialog dlg = builder.show();
            dlg.setCanceledOnTouchOutside(false);
            ((Button)settingsDlgView.findViewById(R.id.button_settings_exit)).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    dlg.dismiss();
                }
            });
            final SeekBar skCompressRate = (SeekBar)settingsDlgView.findViewById(R.id.seekBar_settings_compressrate);
            final TextView tvCompressRate = (TextView)settingsDlgView.findViewById(R.id.textView_settings_compressratwe);
            tvCompressRate.setText(String.valueOf(  Math.round(compress_rate*100f) )  + "%");
            skCompressRate.setProgress(Math.round(compress_rate*100f) );
            skCompressRate.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                    tvCompressRate.setText(String.valueOf(skCompressRate.getProgress()) + "%");
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                    if(skCompressRate.getProgress() < 1){
                        skCompressRate.setProgress(1);
                    }
                    compress_rate = skCompressRate.getProgress()/100f;
                    Config.save("COMPRESSRATE",String.valueOf(compress_rate));
                    Log.v("Compress_rate=", String.valueOf(compress_rate));
                }
            });
            return true;
        }else if(id == R.id.action_multiselection){

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
                imageBitmap = scaleBitmap(imageBitmap,compress_rate);
                ThumbnailActivity.target_image = imageBitmap;
                Intent confirm_screen = new Intent(getApplicationContext(), ThumbnailActivity.class);
                //add data to recentscreened
                RecentItem ri = new RecentItem(last_taken_pictures_name,"",imageBitmap,last_taken_pictures_abpath);
                ThumbnailActivity.aa_recent = aa_recent;
                ThumbnailActivity.ri = ri;
                // goto process activity
                startActivity(confirm_screen);
            }else if( requestCode == REQUEST_PICK_IMAGE ){
                Toast.makeText(this,"open image" + data.getData(), Toast.LENGTH_LONG).show();
                Bitmap bp = getPicture(data.getData());
                bp = scaleBitmap(bp,compress_rate);
                ThumbnailActivity.target_image = bp;
                Intent confirm_screen = new Intent(getApplicationContext(), ThumbnailActivity.class);
                //add data to recentscreened
                String furi = data.getData().toString();
                File f = new File(furi);
                RecentItem ri = new RecentItem(f.getName() ,"",bp,furi);
                ThumbnailActivity.ri = ri;
                ThumbnailActivity.aa_recent = aa_recent;
                // goto process activity
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

    public Bitmap getImage(String path){
        if(path.indexOf("content:") >= 0) {
            return  compressAsJPG(getPicture(Uri.parse(path)));
        }else{
            return compressAsJPG(getPicture(path));
        }
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
        last_taken_pictures_name = imageFileName;
        return image;
    }

    //loading all recent image in the background
    private class LoadAllRecentInBackground extends AsyncTask<SQLiteDatabase, Void, ArrayList<RecentItem>> {

        @Override
        protected ArrayList<RecentItem> doInBackground(SQLiteDatabase[] params) {
            return loadRecentImages(params[0]);
        }

        @Override
        protected void onPostExecute(ArrayList<RecentItem> message) {
            al_recent = message;
            aa_recent = new RecentItemAdaptor(getApplicationContext(),al_recent);
            lvrecent.setAdapter(aa_recent);
        }
        @Override
        protected void onPreExecute() {
            Toast.makeText(getApplicationContext(),"loading recent images...",Toast.LENGTH_LONG).show();
        }
    }

    // open exist files.
    private class openPreiousOne extends AsyncTask<String, Void, Bitmap> {

        final private ProgressDialog loading= new ProgressDialog(CurActivitycontext);

        @Override
        protected Bitmap doInBackground(String... path) {
            return getImage(path[0]);
        }


        @Override
        protected void onPostExecute(Bitmap s) {
            loading.dismiss();
            ThumbnailActivity.target_image = s;
            Intent confirm_screen = new Intent(getApplicationContext(), ThumbnailActivity.class);
            startActivity(confirm_screen);
        }

        @Override
        protected void onPreExecute() {
            loading.setMessage("处理中...");
            loading.show();
            loading.setCancelable(false);
            loading.setCanceledOnTouchOutside(false);
        }
    }

    // load recent data from database and generate thumbnails
    private  ArrayList<RecentItem> loadRecentImages(SQLiteDatabase db){
        ArrayList<RecentItem> al_recent = new ArrayList<>();
        al_recent = dbhelper.getAllRecentItem(db);
        if(al_recent.size() == 0){
            al_recent.add(new RecentItem("No Recent Image Found","null",
                    Bitmap.createBitmap(128,128,Bitmap.Config.ARGB_8888),"null"));
        }else{
            // load all thumbnail of these images
            for(int i=0;i<al_recent.size();i++){
                if(al_recent.get(i).fpath.indexOf("content:") >= 0) {
                    al_recent.get(i).thumbnail = scaleBitmap(getPicture(Uri.parse(al_recent.get(i).fpath)),0.1f);
                }else{
                    al_recent.get(i).thumbnail = scaleBitmap(getPicture(al_recent.get(i).fpath),0.1f);
                }
            }
        }
        Collections.reverse(al_recent);
        return al_recent;
    }

    // check and request permission for the first run
    private void checkAndRequestPermission(){
        // Here, thisActivity is the current activity
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            // Should we show an explanation?
            if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            } else {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1111);
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case 1111: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(this,"Permission Granted.",Toast.LENGTH_LONG).show();
                } else {
                    // permission denied, boo!
                    Toast.makeText(this,"We need WRITE STORAGE permission to work properly!",Toast.LENGTH_LONG).show();
                }
                return;
            }
        }
    }

    private void setListenerOnListView(ListView lv){

        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                RecentItem ri = (RecentItem)adapterView.getAdapter().getItem(i);
                LayoutInflater li = LayoutInflater.from(CurActivitycontext);
                final View TexthistoryDlgView = li.inflate(R.layout.layout_texthistory, null);
                final AlertDialog.Builder builder = new AlertDialog.Builder(CurActivitycontext);
                builder.setView(TexthistoryDlgView);
                final AlertDialog dlg = builder.show();
                dlg.setCanceledOnTouchOutside(false);
                ((EditText)TexthistoryDlgView.findViewById(R.id.editText_texthistory)).setText(ri.data);
                TexthistoryDlgView.findViewById(R.id.button_texthistory_ok).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        dlg.dismiss();
                    }
                });
                TexthistoryDlgView.findViewById(R.id.button_texthistory_copytext).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                        String textout = ((EditText)TexthistoryDlgView.findViewById(R.id.editText_texthistory)).getText().toString();
                        ClipData clip = ClipData.newPlainText("document2docs", textout);
                        clipboard.setPrimaryClip(clip);
                        Snackbar.make(view, "Text has been clipped to the clipboard", Snackbar.LENGTH_LONG).show();
                    }
                });

            }
        });

        lv.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(final AdapterView<?> adapterView, View view, int i, long l) {
                final RecentItem ri = (RecentItem)adapterView.getItemAtPosition(i);
                final PopupMenu pm = new PopupMenu(getApplicationContext(),view);
                pm.getMenuInflater().inflate(R.menu.recent_item_menu, pm.getMenu());
                final View v = view;
                final int position = i;
                pm.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {
                        switch(menuItem.getItemId()){
                            case R.id.menu_recentitem_delete: //delete selected item
                                Snackbar.make(v,"Item deleted.",Snackbar.LENGTH_LONG).show();
                                dbhelper.deleteRecentItem(db,ri.filename);
                                aa_recent.remove(ri);
                                break;
                            case R.id.menu_recentitem_viewdata: //showing data for selected pciture
                                //Snackbar.make(v,"You clicked me!fp="+ri.fpath,Snackbar.LENGTH_LONG).show();
                                final RecentItem ri = (RecentItem)adapterView.getItemAtPosition(position);
                                new openPreiousOne().execute(ri.fpath);
                                break;
                        }
                        return true;
                    }
                });
                pm.show();
                return true;
            }
        });

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    // returnimg: 1 means return all splited images, 2 for return reconized chars, 3 for return words segmentation ;
    public static native String processImageJNI(int[] ipcData,int width,int height,int channels,int returnimg);
}
