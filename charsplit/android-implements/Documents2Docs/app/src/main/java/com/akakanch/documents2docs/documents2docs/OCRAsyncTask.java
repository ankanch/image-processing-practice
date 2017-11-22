package com.akakanch.documents2docs.documents2docs;

import android.app.Activity;
import android.app.ProgressDialog;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.util.Base64;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLEncoder;
import java.util.Iterator;

import javax.net.ssl.HttpsURLConnection;

/**
 * Created by kanch on 11/22/2017.
 */

public class OCRAsyncTask extends AsyncTask {
    private static final String TAG = OCRAsyncTask.class.getName();

    String url = "https://api.ocr.space/parse/image"; // OCR API Endpoints

    private String mApiKey = "90fae1d1ca88957";
    private boolean isOverlayRequired = false;
    private Bitmap mImage;
    private String mLanguage;
    private Activity mActivity;
    private ProgressDialog mProgressDialog;
    private IOCRCallBack mIOCRCallBack;

    public OCRAsyncTask(Activity activity, Bitmap image, String language, IOCRCallBack iOCRCallBack) {
        this.mActivity = activity;
        this.mImage = image;
        this.mLanguage = language;
        this.mIOCRCallBack = iOCRCallBack;
    }

    @Override
    protected void onPreExecute() {
        mProgressDialog = new ProgressDialog(mActivity);
        mProgressDialog.setMessage("processing...");
        mProgressDialog.setCanceledOnTouchOutside(false);
        mProgressDialog.setCancelable(false);
        mProgressDialog.show();
        super.onPreExecute();
    }

    @Override
    protected String doInBackground(Object[] params) {

        try {
            return sendPost(mApiKey, mImage, mLanguage);

        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }
    public static String encodeToBase64(Bitmap image)
    {
        ByteArrayOutputStream byteArrayOS = new ByteArrayOutputStream();
        image.compress(Bitmap.CompressFormat.PNG, 30, byteArrayOS);
        return "data:image/png;base64," + Base64.encodeToString(byteArrayOS.toByteArray(), Base64.DEFAULT).replace(" ","").replace("\n","").replace("\r","");
    }

    private String sendPost(String apiKey, Bitmap image, String language) throws Exception {
        mProgressDialog.setMessage("encoding image...");
        String data = encodeToBase64(image);

        mProgressDialog.setMessage("uploading data...");
        URL obj = new URL(url); // OCR API Endpoints
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        //add request header
        con.setRequestMethod("POST");
        con.setRequestProperty("User-Agent", "Mozilla/5.0");
        con.setRequestProperty("Accept-Language", "en-US,en;q=0.5");


        JSONObject postDataParams = new JSONObject();

        postDataParams.put("apikey", apiKey);//TODO Add your Registered API key
        postDataParams.put("isOverlayRequired", false);
        postDataParams.put("base64Image", data);
        Log.v("base64Image=",data);
        postDataParams.put("language", language);


        // Send post request
        con.setDoOutput(true);
        DataOutputStream wr = new DataOutputStream(con.getOutputStream());
        wr.writeBytes(getPostDataString(postDataParams));
        wr.flush();
        wr.close();

        BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
        String inputLine;
        StringBuffer response = new StringBuffer();

        while ((inputLine = in.readLine()) != null) {
            response.append(inputLine);
        }
        in.close();

        //return result
        return String.valueOf(response);
    }

    @Override
    protected void onPostExecute(Object result) {
        super.onPostExecute(result);
        mProgressDialog.setMessage("processing...");
        String text = "";
        if(result == null){
            text = "error : null pointer got";
        }else {
            Log.v("Response:", (String) result);
            if (mProgressDialog != null && mProgressDialog.isShowing())
                mProgressDialog.dismiss();
            try {
                JSONObject re = new JSONObject((String) result);
                if ((re).getBoolean("IsErroredOnProcessing") == false) {
                    JSONArray ja = re.getJSONArray("ParsedResults");
                    text = ja.getJSONObject(0).getString("ParsedText");
                } else {
                    String error = re.getJSONArray("ErrorMessage").getString(0);
                    text = "error occured when processing:" + error;
                }
            } catch (JSONException e) {
                e.printStackTrace();
                Log.v("error-in-post-process", e.getMessage());
                text = "error occured when processing.";
            }
        }
        mIOCRCallBack.getOCRCallBackResult(text);
    }

    public String getPostDataString(JSONObject params) throws Exception {

        StringBuilder result = new StringBuilder();
        boolean first = true;

        Iterator<String> itr = params.keys();

        while (itr.hasNext()) {

            String key = itr.next();
            Object value = params.get(key);

            if (first)
                first = false;
            else
                result.append("&");

            result.append(URLEncoder.encode(key, "UTF-8"));
            result.append("=");
            result.append(URLEncoder.encode(value.toString(), "UTF-8"));

        }
        return result.toString();
    }
}
