package com.trexalarm.horay.trexalarm;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Vibrator;
import android.support.annotation.Nullable;

/**
 * Created by HoRay on 11/7/2017.
 */


public class backgroundService extends Service{

    Vibrator vibrator;
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        //// Each element then alternates between delay, vibrate, sleep, vibrate, sleep
           //Ex. notificationBuilder.setVibrate(new long[] { 1000, 1000, 1000, 1000, 1000});
        vibrator = (Vibrator)getSystemService(VIBRATOR_SERVICE);

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        vibrator.vibrate(2000);
        return super.onStartCommand(intent, flags, startId);

    }


}
