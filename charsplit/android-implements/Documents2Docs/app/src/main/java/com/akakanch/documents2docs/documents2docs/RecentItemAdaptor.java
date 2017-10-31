package com.akakanch.documents2docs.documents2docs;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by kanch on 10/31/2017.
 */

public class RecentItemAdaptor extends ArrayAdapter<RecentItem> {
    private Context ct;

    public RecentItemAdaptor(Context context, ArrayList<RecentItem> users) {
        super(context, 0, users);
        ct = context;
    }

    @Override
    public View getView(int position,  View convertView, ViewGroup parent) {
        //检查视图是否被复用，否则用view填充
        if (convertView == null) {
            convertView = LayoutInflater.from(getContext()).inflate(R.layout.layout_recentitem, parent, false);
        }
        TextView tvName = convertView.findViewById(R.id.textView_recentname);
        ImageView ivThumbnail = convertView.findViewById(R.id.imageView_recentthumbnail);
        RecentItem ri = getItem(position);
        tvName.setText(ri.filename);
        ivThumbnail.setImageBitmap(ri.thumbnail);



        return convertView;
    }
}
