package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceHolder;
import android.widget.TextView;
import java.io.File;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.View;
import android.app.Activity;
import androidx.core.app.ActivityCompat;
import android.content.pm.PackageManager;

public class MainActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private SurfaceHolder surfaceHolder;
    //先定义
    private static final int REQUEST_EXTERNAL_STORAGE = 1;

    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE" };

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        verifyStoragePermissions(this);
        surfaceView = (SurfaceView) findViewById(R.id.surface_view);
        surfaceHolder = surfaceView.getHolder();
        //String path = ;
        File file = new File("/sdcard/Movies/ScreenCaptures/20200729-234214.mp4");
        String videoPath = "/sdcard/Movies/ScreenCaptures/20200729-234214.mp4";
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText("hello ffmpeg");
    }

    public void play(View view) {
        String videoPath = "/sdcard/Movies/ScreenCaptures/20200729-234214.mp4";
//        File file = new File(videoPath);
//        String inputurl = file.getAbsolutePath();
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI(videoPath,surfaceHolder.getSurface()));
    }

    //然后通过一个函数来申请
    public static void verifyStoragePermissions(Activity activity) {
        try {
            //检测是否有写的权限
            int permission = ActivityCompat.checkSelfPermission(activity,
                    "android.permission.WRITE_EXTERNAL_STORAGE");
            if (permission != PackageManager.PERMISSION_GRANTED) {
                // 没有写的权限，去申请写的权限，会弹出对话框
                ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE,REQUEST_EXTERNAL_STORAGE);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI(String inputurl,Surface surface);
}
