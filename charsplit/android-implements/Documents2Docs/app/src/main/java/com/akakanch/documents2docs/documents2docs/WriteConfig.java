package com.akakanch.documents2docs.documents2docs;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by kanch on 11/22/2017.
 */

public class WriteConfig {
    public static String FIELD_SPLIT = "<@SPxFIELD@>";
    public static String ROW_SPLIT = "<@SPxROW@>";
    private Context context;

    public WriteConfig(Context ct) {
        context  = ct;
    }

    public void save( String key, String value){
        SharedPreferences sharedPref = context.getSharedPreferences("S",Context.MODE_PRIVATE) ;//getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(key, value);
        editor.commit();
    }

    public String read(String key){
        SharedPreferences sharedPref = context.getSharedPreferences("S",Context.MODE_PRIVATE) ;
        String defaultValue = "";
        String value = sharedPref.getString(key, defaultValue);
        return value;
    }

    public String joinToGroup(String[] al){
        String result = "";
        for(int i=0;i<al.length-1;i++){
            result += al[i] + FIELD_SPLIT;
        }
        result += al[al.length-1];
        return result;
    }

    public void append(String key, String newvalue){
        String orginal = read(key);
        save(key,orginal + ROW_SPLIT + newvalue);
    }

    public String[][] parse(String raw){
        String[] rows = raw.split(ROW_SPLIT);
        String[][] data = new String[rows.length][];
        for(int i=0;i<rows.length;i++){
            String[] row = rows[i].split(FIELD_SPLIT);
            data[i] = row;
        }
        return data;
    }
}
