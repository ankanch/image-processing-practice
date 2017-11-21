package com.akakanch.documents2docs.documents2docs;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import java.util.ArrayList;

/**
 * Created by kanch on 10/31/2017.
 */

public class DatabaseHelper extends SQLiteOpenHelper {
    // If you change the database schema, you must increment the database version.
    public static final int DATABASE_VERSION = 1;
    public static final String DATABASE_NAME = "D2DbK.db";



    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }
    public void onCreate(SQLiteDatabase db) {
        String SQL_CREATE_ENTRIES =
                "CREATE TABLE RECENTLIST ( ID INTEGER PRIMARY KEY," +
                        "FILENAME TEXT," +
                        "DATA TEXT," +
                        "PATH TEXT)";
        db.execSQL(SQL_CREATE_ENTRIES);
    }
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // This database is only a cache for online data, so its upgrade policy is
        // to simply to discard the data and start over
        db.execSQL("");
        onCreate(db);
    }
    public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        onUpgrade(db, oldVersion, newVersion);
    }

    public void insertRecentItem(SQLiteDatabase db,RecentItem ri){
        String SQL_INSERT_RECENT_ITEM =
                "INSERT INTO RECENTLIST(FILENAME,DATA,PATH) VALUES(\"" +
                        ri.filename + "\",\""  +
                        ri.data + "\",\""   +
                        ri.fpath +"\")";
        db.execSQL(SQL_INSERT_RECENT_ITEM);
    }

    public ArrayList<RecentItem> getAllRecentItem(SQLiteDatabase db){
        ArrayList<RecentItem> alri = new ArrayList<RecentItem>();
        Cursor cursor = db.rawQuery("select * from RECENTLIST",null);
        if (cursor.moveToFirst()) {
            while (!cursor.isAfterLast()) {
                String filename = cursor.getString(cursor.getColumnIndex("FILENAME"));
                String path = cursor.getString(cursor.getColumnIndex("PATH"));
                String data =  cursor.getString(cursor.getColumnIndex("DATA"));

                alri.add(new RecentItem(filename,data,null,path));
                cursor.moveToNext();
            }
        }
        return alri;
    }

    public void deleteRecentItem(SQLiteDatabase db,String filename){
        String SQL_DELETE_ITEM = "DELETE FROM RECENTLIST WHERE FILENAME=\""+ filename +"\"";
        db.execSQL(SQL_DELETE_ITEM);
    }



}
