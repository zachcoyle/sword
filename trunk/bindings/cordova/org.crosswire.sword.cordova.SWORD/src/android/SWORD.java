/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
package org.crosswire.sword.cordova;

import java.util.TimeZone;
import java.util.Vector;
import java.util.Map;
import java.util.HashMap;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

import org.apache.cordova.CordovaWebView;
import org.apache.cordova.CallbackContext;
import org.apache.cordova.CordovaPlugin;
import org.apache.cordova.PluginResult;
import org.apache.cordova.CordovaInterface;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import org.crosswire.android.sword.SWMgr;
import org.crosswire.android.sword.AndroidMgr;
import org.crosswire.android.sword.SWModule;
import org.crosswire.android.sword.InstallMgr;

import android.provider.Settings;
import android.util.Log;
import android.content.Intent;
import android.app.Activity;

public class SWORD extends CordovaPlugin {
	public static final String TAG = "SWORD";
	public static SWMgr mgr = null;
	public static InstallMgr installMgr = null;
	private CallbackContext installReporterContext = null;
	private CallbackContext searchReporterContext = null;
	private CallbackContext renderChapterContext = null;
	private CallbackContext sendContext = null;

	/**
	* Constructor.
	*/
	public SWORD() {
	}

	/**
	* Sets the context of the Command. This can then be used to do things like
	* get file paths associated with the Activity.
	*
	* @param cordova The context of the main Activity.
	* @param webView The CordovaWebView Cordova is running in.
	*/
	public void initialize(CordovaInterface cordova, CordovaWebView webView) {
		super.initialize(cordova, webView);
		mgr = new AndroidMgr(cordova.getActivity().getApplication());
		installMgr = new InstallMgr();
		installReporterContext = null;
		searchReporterContext = null;
		renderChapterContext = null;
	}

	/**
	* Executes the request and returns PluginResult.
	*
	* @param action            The action to execute.
	* @param args              JSONArry of arguments for the plugin.
	* @param callbackContext   The callback id used when calling back into JavaScript.
	* @return                  True if the action was valid, false if not.
	*/
	public boolean execute(String action, JSONArray args, final CallbackContext callbackContext) throws JSONException {
		if (action.equals("initSWORD")) {
			JSONObject r = new JSONObject();
			r.put("version", mgr.version());
			callbackContext.success(r);
		}
		else if (action.equals("echo")) {
			echo(callbackContext, args.getString(0));
		}
		else if (action.equals("InstallMgr_setUserDisclaimerConfirmed")) {
			installMgr.setUserDisclaimerConfirmed();
			callbackContext.success();
		}
		else if (action.equals("InstallMgr_syncConfig")) {
			cordova.getThreadPool().execute(new Runnable() {
				@Override
				public void run() {
					installMgr.syncConfig();
					installMgr.reInit();
					callbackContext.success();
				}
			});
		}
		else if (action.equals("InstallMgr_getRemoteSources")) {
			JSONArray r = new JSONArray();
			String sources[] = installMgr.getRemoteSources();
			for (String s : sources) {
				r.put(s);
			}
			callbackContext.success(r);
		}
		else if (action.equals("InstallMgr_refreshRemoteSource")) {
			final String repo = args.getString(0);
			cordova.getThreadPool().execute(new Runnable() {
				@Override
				public void run() {
					installMgr.refreshRemoteSource(repo);
					callbackContext.success();
				}
			});
		}
		else if (action.equals("InstallMgr_getRemoteModInfoList")) {
			JSONArray r = new JSONArray();
			for (SWMgr.ModInfo mi : installMgr.getRemoteModInfoList(args.getString(0))) {
				JSONObject m = new JSONObject();
				m.put("name", mi.name);
				m.put("description", mi.description);
				m.put("category", mi.category);
				m.put("language", mi.language);
				m.put("version", mi.version);
				m.put("delta", mi.delta);
				r.put(m);
			}
			callbackContext.success(r);
		}
		else if (action.equals("InstallMgr_remoteInstallModule")) {
			this.installReporterContext = callbackContext;
			final String repo = args.getString(0);
			final String modName = args.getString(1);
			cordova.getThreadPool().execute(new Runnable() {
			    @Override
			    public void run() {

				installMgr.remoteInstallModule(repo, modName, new InstallMgr.InstallProgressReporter() {
					public void update(long totalBytes, long completedBytes) {
	/*
	// callback({ status : preStatus|update|complete, totalBytes : n, completedBytes : n, message : displayable });
	*/
						try {

							JSONObject m = new JSONObject();
							m.put("status", "update");
							m.put("totalBytes", totalBytes);
							m.put("completedBytes", completedBytes);
							if (installReporterContext != null) {
								PluginResult result = new PluginResult(PluginResult.Status.OK, m);
								result.setKeepCallback(true);
								installReporterContext.sendPluginResult(result);
							}
						}
						catch (Exception e) { e.printStackTrace(); }
					}
					public void preStatus(long totalBytes, long completedBytes, String message) {
						try {
							JSONObject m = new JSONObject();
							m.put("status", ("Complete".equals(message)) ? "complete" : "preStatus");
							m.put("totalBytes", totalBytes);
							m.put("completedBytes", completedBytes);
							m.put("message", message);
							if (installReporterContext != null) {
								PluginResult result = new PluginResult(PluginResult.Status.OK, m);
								result.setKeepCallback(!"Complete".equals(message));
								installReporterContext.sendPluginResult(result);
							}
							if ("Complete".equals(message)) {
								// let's be sure we see our new module
								mgr.reInit();
								// and clear out context
								installReporterContext = null;
							}
						}
						catch (Exception e) { e.printStackTrace(); }
					}
				});

			    }
			});

			PluginResult result = new PluginResult(PluginResult.Status.OK);
			result.setKeepCallback(true);
			callbackContext.sendPluginResult(result);
		}
		else if (action.equals("InstallMgr_uninstallModule")) {
			installMgr.uninstallModule(args.getString(0));
			callbackContext.success();
		}
		else if (action.equals("SWMgr_getExtraConfigSections")) {
			JSONArray r = new JSONArray();
			for (String s : mgr.getExtraConfigSections()) {
				r.put(s);
			}
			callbackContext.success(r);
		}
		else if (action.equals("SWMgr_getExtraConfigKeys")) {
			JSONArray r = new JSONArray();
			for (String s : mgr.getExtraConfigKeys(args.getString(0))) {
				r.put(s);
			}
			callbackContext.success(r);
		}
		else if (action.equals("SWMgr_getExtraConfigValue")) {
			callbackContext.success(mgr.getExtraConfigValue(args.getString(0), args.getString(1)));
		}
		else if (action.equals("SWMgr_setExtraConfigValue")) {
			mgr.setExtraConfigValue(args.getString(0), args.getString(1), args.getString(2));
			callbackContext.success();
		}
		else if (action.equals("SWMgr_addExtraConfig")) {
			JSONArray r = new JSONArray();
			for (String s : mgr.addExtraConfig(args.getString(0))) {
				r.put(s);
			}
			callbackContext.success(r);
		}
		else if (action.equals("SWMgr_getModInfoList")) {
			JSONArray r = new JSONArray();
			for (SWMgr.ModInfo mi : mgr.getModInfoList()) {
				JSONObject m = new JSONObject();
				m.put("name", mi.name);
				m.put("description", mi.description);
				m.put("category", mi.category);
				m.put("language", mi.language);
				m.put("version", mi.version);
				m.put("delta", mi.delta);
				r.put(m);
			}
			callbackContext.success(r);
		}
		else if (action.equals("SWMgr_getModuleByName")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			// didn't find module is not an error
			if (mod == null) { callbackContext.success(); return true; }

			JSONObject m = new JSONObject();
			m.put("name", mod.getName());
			m.put("description", mod.getDescription());
			m.put("category", mod.getCategory());
			m.put("direction", mod.getConfigEntry("Direction"));
			m.put("language", mod.getConfigEntry("Lang"));
			m.put("font", mod.getConfigEntry("Font"));
			m.put("shortCopyright", mod.getConfigEntry("ShortCopyright"));
			m.put("shortPromo", mod.getConfigEntry("ShortPromo"));
			callbackContext.success(m);
		}
		else if (action.equals("SWModule_setKeyText")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			mod.setKeyText(args.getString(1));
			callbackContext.success();
		}
		else if (action.equals("SWModule_getKeyText")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			callbackContext.success(mod.getKeyText());
		}
		else if (action.equals("SWModule_search")) {
			this.searchReporterContext = callbackContext;
			final SWModule mod = mgr.getModuleByName(args.getString(0));
			final String expression = args.getString(1);
			final int searchType = args.getInt(2);
			final long flags = args.getLong(3);
			final String scope = JSONObject.NULL.equals(args.getString(4)) || "null".equals(args.getString(4)) ? null : args.getString(4);
			cordova.getThreadPool().execute(new Runnable() {
			    @Override
			    public void run() {

				SWModule.SearchHit[] results = mod.search(expression, searchType, flags, scope, new SWModule.SearchProgressReporter() {
					public void progressReport(int percent) {
						try {
							JSONObject m = new JSONObject();
							m.put("status", "update");
							m.put("percent", percent);
							if (searchReporterContext != null) {
								PluginResult result = new PluginResult(PluginResult.Status.OK, m);
								result.setKeepCallback(true);
								searchReporterContext.sendPluginResult(result);
							}
						}
						catch (Exception e) { e.printStackTrace(); }
					}
				});
				try {
					JSONObject m = new JSONObject();
					m.put("status", "complete");
					m.put("percent", 100);
					JSONArray r = new JSONArray();
					for (SWModule.SearchHit s : results) {
						JSONObject re = new JSONObject();
						re.put("key", s.key);
						re.put("score", s.score);
						r.put(re);
					}
					m.put("results", r);
					if (searchReporterContext != null) {
						PluginResult result = new PluginResult(PluginResult.Status.OK, m);
						result.setKeepCallback(false);
						searchReporterContext.sendPluginResult(result);
					}
				}
				catch (Exception e) { e.printStackTrace(); }
				searchReporterContext = null;
			    }
			});

			PluginResult result = new PluginResult(PluginResult.Status.OK);
			result.setKeepCallback(true);
			callbackContext.sendPluginResult(result);
		}
		else if (action.equals("SWModule_getRenderChapter")) {
			this.renderChapterContext = callbackContext;

			final SWModule masterMod = mgr.getModuleByName(args.getString(0));
			final SWModule mod = mgr.getModuleByName(args.getString(1));
			if (masterMod == null) { callbackContext.error("couldn't find master module: " + args.getString(0)); return true; }
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(1)); return true; }

			cordova.getThreadPool().execute(new Runnable() {
			    @Override
			    public void run() {

				JSONArray r = new JSONArray();
				try {
					r = getRenderChapter(masterMod, mod);
				} catch (JSONException e) { e.printStackTrace(); }
				PluginResult result = new PluginResult(PluginResult.Status.OK, r);
				result.setKeepCallback(false);
				if (renderChapterContext != null) {
					renderChapterContext.sendPluginResult(result);
					renderChapterContext = null;
				}
			    }
			});

			PluginResult result = new PluginResult(PluginResult.Status.NO_RESULT);
			result.setKeepCallback(true);
			callbackContext.sendPluginResult(result);
		}
		else if (action.equals("SWMgr_registerBibleSyncListener")) {
			final CallbackContext bibleSyncListener = callbackContext;

			cordova.getThreadPool().execute(new Runnable() {
			    @Override
			    public void run() {
				mgr.registerBibleSyncListener(new SWMgr.BibleSyncListener() {
					public void messageReceived(String message) {
						PluginResult result = new PluginResult(PluginResult.Status.OK, message);
						result.setKeepCallback(true);
						bibleSyncListener.sendPluginResult(result);
					}
				});
			    }
			});

			PluginResult result = new PluginResult(PluginResult.Status.NO_RESULT);
			result.setKeepCallback(true);
			callbackContext.sendPluginResult(result);
		}
		else if (action.equals("SWMgr_setJavascript")) {
			boolean val = args.getBoolean(0);
			mgr.setJavascript(val);
			callbackContext.success();
		}
		else if (action.equals("SWMgr_sendBibleSyncMessage")) {
			String osisRef = args.getString(0);
			mgr.sendBibleSyncMessage(osisRef);
			callbackContext.success();
		}
		else if (action.equals("SWModule_getRenderText")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			callbackContext.success(mod.getRenderText());
		}
		else if (action.equals("SWModule_sendText")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			this.sendContext = callbackContext;
			sendVerse(mod, new String[] { mod.getKeyText() }, cordova, this);
			PluginResult result = new PluginResult(PluginResult.Status.NO_RESULT);
			result.setKeepCallback(true);
			callbackContext.sendPluginResult(result);
		}
		else if (action.equals("SWModule_getRenderHeader")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			callbackContext.success(mod.getRenderHeader());
		}
		else if (action.equals("SWModule_getKeyChildren")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			JSONArray r = new JSONArray();
			for (String k : mod.getKeyChildren()) {
				r.put(k);
			}
			callbackContext.success(r);
		}
		else if (action.equals("SWModule_getConfigEntry")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			callbackContext.success(mod.getConfigEntry(args.getString(1)));
		}
		else if (action.equals("SWModule_getEntryAttribute")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			JSONArray r = new JSONArray();
			for (String k : mod.getEntryAttribute(args.getString(1), args.getString(2), args.getString(3), args.getBoolean(4))) {
				r.put(k);
			}
			callbackContext.success(r);
		}
		else if (action.equals("SWModule_popError")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			callbackContext.success((int)mod.error());
		}
		else if (action.equals("SWModule_next")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			mod.next();
			callbackContext.success();
		}
		else if (action.equals("SWModule_previous")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			mod.previous();
			callbackContext.success();
		}
		else if (action.equals("SWModule_begin")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			mod.begin();
			callbackContext.success();
		}
		else if (action.equals("SWModule_getVerseKey")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			callbackContext.success(getVerseKey(mod.getKeyChildren()));
		}
		else if (action.equals("SWModule_getBookNames")) {
			SWModule mod = mgr.getModuleByName(args.getString(0));
			if (mod == null) { callbackContext.error("couldn't find module: " + args.getString(0)); return true; }
			JSONArray r = new JSONArray();
			for (String b : getBookNames(mod)) {
				r.put(b);
			}
			callbackContext.success(r);
		}
		else if (action.equals("HTTPUtils_makeRequest")) {
			final CallbackContext makeRequestContext = callbackContext;

			final String url      = args.getString(0);
			final String postData = args.getString(1);
			final int method      = args.getInt(2);
Log.d(TAG, "makeRequest(url: " + url + ", postData: " + postData + ", method: " + method);

			cordova.getThreadPool().execute(new Runnable() {
				@Override
				public void run() {
					String response = makeRequest(url, postData, method, null);
					PluginResult result = new PluginResult(PluginResult.Status.OK, response);
					result.setKeepCallback(false);
					makeRequestContext.sendPluginResult(result);
				}
			});

			PluginResult result = new PluginResult(PluginResult.Status.NO_RESULT);
			result.setKeepCallback(true);
			callbackContext.sendPluginResult(result);
		}
		else return false;

		return true;
	}

	JSONObject getVerseKey(String keyChildren[]) throws JSONException {
		JSONObject retVal = new JSONObject();
		if (keyChildren != null && keyChildren.length > 9) {
			retVal.put("testament",  Integer.parseInt(keyChildren[SWModule.VERSEKEY_TESTAMENT]));
			retVal.put("book",       Integer.parseInt(keyChildren[SWModule.VERSEKEY_BOOK]));
			retVal.put("chapter",    Integer.parseInt(keyChildren[SWModule.VERSEKEY_CHAPTER]));
			retVal.put("verse",      Integer.parseInt(keyChildren[SWModule.VERSEKEY_VERSE]));
			retVal.put("chapterMax", Integer.parseInt(keyChildren[SWModule.VERSEKEY_CHAPTERMAX]));
			retVal.put("verseMax",   Integer.parseInt(keyChildren[SWModule.VERSEKEY_VERSEMAX]));
			retVal.put("bookName",   keyChildren[SWModule.VERSEKEY_BOOKNAME]);
			retVal.put("osisRef",    keyChildren[SWModule.VERSEKEY_OSISREF]);
			retVal.put("shortText",  keyChildren[SWModule.VERSEKEY_SHORTTEXT]);
			retVal.put("bookAbbrev", keyChildren[SWModule.VERSEKEY_BOOKABBREV]);
		}
		return retVal;
	}

	public JSONArray getRenderChapter(SWModule masterMod, SWModule mod) throws JSONException {
		JSONArray r = new JSONArray();

		// save our current keys so we can set our mods back when we're done
		String saveMasterKey = masterMod.getKeyText();
		String saveKey = mod.getKeyText();

		String currentKey[]   = masterMod.getKeyChildren();

		masterMod.setKeyText(currentKey[SWModule.VERSEKEY_BOOKABBREV]+"."+currentKey[SWModule.VERSEKEY_CHAPTER]+".1");

		String [] verseKey = masterMod.getKeyChildren();
		while (
				   masterMod.error() == 0
				&& currentKey[SWModule.VERSEKEY_BOOK].equals(verseKey[SWModule.VERSEKEY_BOOK])
				&& currentKey[SWModule.VERSEKEY_CHAPTER].equals(verseKey[SWModule.VERSEKEY_CHAPTER])) {

			mod.setKeyText(verseKey[SWModule.VERSEKEY_OSISREF]);
			char error = mod.error();
			JSONObject v = new JSONObject();
			if (((int)error) == 0) {
				v.put("verse", getVerseKey(mod.getKeyChildren()));
				String preVerse = "";
				for (String h : mod.getEntryAttribute("Heading", "Preverse", "", true)) {
					preVerse += h;
				}
				v.put("preVerse", preVerse);
				v.put("text", mod.getRenderText());
			}
			else {
/* null removes entry
				v.put("verse", null);
				v.put("preVerse", null);
				v.put("text", null);
*/
			}
			r.put(v);
			masterMod.next();
			verseKey = masterMod.getKeyChildren();
		}

		masterMod.setKeyText(saveMasterKey);
		mod.setKeyText(saveKey);

		return r;
	}


	public Vector<String> getBookNames(SWModule mod) {
		Vector<String> books = new Vector<String>();
		for (mod.begin(); mod.error() == 0; mod.setKeyText("+book")) {
			books.add(mod.getKeyChildren()[9]);
		}
		return books;
	}

	//--------------------------------------------------------------------------
	// LOCAL METHODS
	//--------------------------------------------------------------------------

	public void echo(CallbackContext callbackContext, String message) {
		if (message != null && message.length() > 0) {
			callbackContext.success(message);
		}
		else {
			callbackContext.error("expected 1 arg");
		}
	}

	public static final int METHOD_GET  = 0;
	public static final int METHOD_POST = 1;
	public static final String METHODS_TEXT[] = { "GET", "POST" };

	public static String makeRequest(String url, String postData, int method, Map<String, String> headers) {
		HttpURLConnection connection = null;

		Map<String, String> defaultHeaders = new HashMap<String, String>();
		defaultHeaders.put("Accept", "*/*");
//		defaultHeaders.put("Content-Type", "application/xml");
	 	defaultHeaders.put("Content-Type", "application/x-www-form-urlencoded");
		if (method != METHOD_GET && postData != null) defaultHeaders.put("Content-Length", Integer.toString(postData.length()));

		try {
			if (headers != null) defaultHeaders.putAll(headers);

			if (method == METHOD_GET && postData != null && postData.length() > 0) {
				// some sanity checks for appending GET params to URL
				if (url.indexOf("?") < 0) if (!postData.startsWith("?")) url += "?";
				else if (!url.endsWith("&") && !postData.startsWith("&")) url += "&";

				url += postData;
			}

			connection = (HttpURLConnection) new URL(url).openConnection();
			connection.setRequestMethod(METHODS_TEXT[method]);
			for (String k : defaultHeaders.keySet()) {
				connection.setRequestProperty(k, defaultHeaders.get(k));
			}
			if (method == METHOD_POST) {
				connection.setDoOutput(true);
				if (postData != null) {
					DataOutputStream dos = new DataOutputStream(connection.getOutputStream());
					dos.writeBytes(postData);
					dos.flush();
					dos.close();
				}
			}
			int responseCode = connection.getResponseCode();
			BufferedReader br = new BufferedReader(new InputStreamReader(connection.getInputStream()));
			StringBuilder response = new StringBuilder();
			String line;
			while ((line = br.readLine()) != null) {
				response.append(line);
			}
			/* do something special if success code?
			if (responseCode >= 200 && responseCode < 300) {
			}
			else throw new Exception("HTTP Response: " + responseCode);
			*/
			return response.toString();
		}
		catch (Exception e) { e.printStackTrace(); }
		return null;
	}
	public static void sendVerse(final SWModule mod, final String verse[], final CordovaInterface cordova, final CordovaPlugin plugin) {
		cordova.getThreadPool().execute(new Runnable() {
			public void run() {
				String subject = mod.getKeyChildren()[SWModule.VERSEKEY_SHORTTEXT] + " (" + mod.getName() + ")";
				String message = mod.getStripText() + " --" +subject;
				final Intent sendIntent = new Intent(Intent.ACTION_SEND);
				sendIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);

				sendIntent.setType("text/plain");

				sendIntent.putExtra(Intent.EXTRA_SUBJECT, subject);
				sendIntent.putExtra(android.content.Intent.EXTRA_TEXT, message);

				cordova.startActivityForResult(plugin, Intent.createChooser(sendIntent, null), 1);
			}
		});
	}

	public void onActivityResult(int requestCode, int resultCode, Intent intent) {
		switch (requestCode) {
		case 1: 
			sendContext.sendPluginResult(new PluginResult(PluginResult.Status.OK, resultCode == Activity.RESULT_OK));
		}
		super.onActivityResult(requestCode, resultCode, intent);
	}

}
