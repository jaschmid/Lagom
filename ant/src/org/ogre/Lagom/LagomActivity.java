package org.ogre.Lagom;

import java.io.IOException;

import com.nvidia.devtech.NvGLES2Activity;

import android.content.pm.PackageInfo;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.Context;
import android.util.Log;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class LagomActivity extends NvGLES2Activity {	
	private static final String TAG = "LagomActivity";

	@Override
	public native boolean render(int drawWidth, int drawHeight, boolean forceRedraw);

	@Override
	public native void cleanup();

	@Override
	public boolean init()
	{
		PackageInfo info = null;
		try 
		{
		   info = getPackageManager().getPackageInfo("org.ogre.Lagom", 0);
		} 
		catch( PackageManager.NameNotFoundException e ) 
		{
		   Log.e( TAG, e.toString() );
		   return false;
		}
		return initLagom( info.applicationInfo.sourceDir );
	}	

	@Override
	public native boolean inputEvent(int action, float x, float y, MotionEvent event);

	@Override
	public native boolean keyEvent(int action, int unicodeChar, int keyCode, KeyEvent event);
	
	public native boolean initLagom(String _package);

	public native void setOffsets(int x, int y);
	
	public void onCreate(Bundle savedInstanceState){
		// Set up asset manager
			
		// Call parent
		super.onCreate(savedInstanceState);
		
		//Force landscape
	    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	}
	
	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		int screenHeight = this.getWindowManager().getDefaultDisplay().getHeight();
		int viewHeight = surfaceView.getHeight();
		
		// Use the difference as the cursor offset
		setOffsets(0, viewHeight - screenHeight);
		
		super.onWindowFocusChanged(hasFocus);
	}
	
	static
    {
		System.loadLibrary("fmodex");
		System.loadLibrary("Lagom");
    }
}