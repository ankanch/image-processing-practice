package com.akakanch.documents2docs.documents2docs;

import android.graphics.Bitmap;

/**
 * Created by kanch on 10/31/2017.
 */

public class RecentItem {

    public String filename;
    public String fpath;
    public Bitmap thumbnail;
    public String data;

    public RecentItem() {
        filename = "";
        thumbnail = null;
        data = "";
        fpath = "";
    }

    public RecentItem(String filename,String data,Bitmap bp,String path) {
        this.filename = filename;
        this.thumbnail = bp;
        this.data = data;
        this.fpath = path;
    }

}
