#include "AndroidArchive.h"

#include <OgreStringConverter.h>
#include <OgreLogManager.h>

#include <android/log.h>

static const char assetsDirectory[] = "assets/";

#define  LOG_TAG    "AndroidArchive"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

namespace Ogre{
	
	
	AndroidArchive::AndroidArchive(const std::string& archiveName,const String& name, const String& archType, JNIEnv *env)
		:ZipArchive(name, archType,&pluginIo), mEnv(env),apk_name(archiveName)
	{
		//init pluginIO
		zzip_init_io (& pluginIo, 0);
	}

	AndroidArchive::~AndroidArchive()
	{
		unload();
	}
    //-----------------------------------------------------------------------
    void AndroidArchive::load()
    {
		LOGI("Opening Archive: %s as %s",apk_name.c_str(),(const char*)mName.c_str());

		OGRE_LOCK_AUTO_MUTEX
        if (!mZzipDir)
        {
            zzip_error_t zzipError;
            mZzipDir = zzip_dir_open_ext_io(apk_name.c_str(), &zzipError, 0, mPluginIo);
            checkZzipError(zzipError, "opening archive");

            // Cache names
            ZZIP_DIRENT zzipEntry;
            while (zzip_dir_read(mZzipDir, &zzipEntry))
            {
                FileInfo info;
				info.archive = this;
                // Get basename / path
                StringUtil::splitFilename(zzipEntry.d_name, info.basename, info.path);
                info.filename = zzipEntry.d_name;
                // Get sizes
                info.compressedSize = static_cast<size_t>(zzipEntry.d_csize);
                info.uncompressedSize = static_cast<size_t>(zzipEntry.st_size);
                // folder entries
                if (info.basename.empty())
                {
                    info.filename = info.filename.substr (0, info.filename.length () - 1);
                    StringUtil::splitFilename(info.filename, info.basename, info.path);
                    // Set compressed size to -1 for folders; anyway nobody will check
                    // the compressed size of a folder, and if he does, its useless anyway
                    info.compressedSize = size_t (-1);
                }
                else
                {
                    info.filename = info.basename;
                }
				
				if(info.path == Ogre::String(assetsDirectory))
				{
					LOGI("ANDROID ARCHIVE: Caching File: %s",info.filename.c_str());

					mFileList.push_back(info);
				}

            }

        }
    }
    //-----------------------------------------------------------------------
    void AndroidArchive::unload()
    {
		OGRE_LOCK_AUTO_MUTEX
        if (mZzipDir)
        {
            zzip_dir_close(mZzipDir);
            mZzipDir = 0;
            mFileList.clear();
        }
    
    }
	//------------------------------------------------------------------------
	
    StringVectorPtr AndroidArchive::list(bool recursive, bool dirs)
    {
		OGRE_LOCK_AUTO_MUTEX
        StringVectorPtr ret = StringVectorPtr(OGRE_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);

        FileInfoList::iterator i, iend;
        iend = mFileList.end();
        for (i = mFileList.begin(); i != iend; ++i)
            if ((dirs == (i->compressedSize == size_t (-1))))
                ret->push_back(i->basename);
		
        LogManager::getSingleton().logMessage("Android Archive: getting list of " + Ogre::StringConverter::toString(ret->size()) + " files");
        return ret;
    }
    //-----------------------------------------------------------------------
    FileInfoListPtr AndroidArchive::listFileInfo(bool recursive, bool dirs)
    {
		OGRE_LOCK_AUTO_MUTEX
        FileInfoList* fil = OGRE_NEW_T(FileInfoList, MEMCATEGORY_GENERAL)();
        FileInfoList::const_iterator i, iend;
        iend = mFileList.end();
        for (i = mFileList.begin(); i != iend; ++i)
            if ((dirs == (i->compressedSize == size_t (-1))))
			{
                fil->push_back(*i);
				fil->back().filename = fil->back().basename;
				fil->back().path = "";
			}
		
        LogManager::getSingleton().logMessage("Android Archive: getting file info of " + Ogre::StringConverter::toString(fil->size()) + " files");
        return FileInfoListPtr(fil, SPFM_DELETE_T);
    }
    //-----------------------------------------------------------------------
	DataStreamPtr AndroidArchive::open(const String& filename, bool readOnly) const
	{
		return ZipArchive::open(assetsDirectory + filename,readOnly);
	}
	StringVectorPtr AndroidArchive::find(const String& pattern, bool recursive, bool dirs)
	{
		OGRE_LOCK_AUTO_MUTEX
        StringVectorPtr ret = StringVectorPtr(OGRE_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);
                          (pattern.find ('\\') != String::npos);

        FileInfoList::iterator i, iend;
        iend = mFileList.end();
        for (i = mFileList.begin(); i != iend; ++i)
            if ((dirs == (i->compressedSize == size_t (-1))))
                // Check basename matches pattern (zip is case insensitive)
                if (StringUtil::match(i->basename, pattern, false))
                    ret->push_back(i->basename);
		
        LogManager::getSingleton().logMessage("ZZIP: finding file " +pattern+ ", found " + Ogre::StringConverter::toString(ret->size()) + " files");
        return ret;

	}
	FileInfoListPtr AndroidArchive::findFileInfo(const String& pattern, bool recursive, bool dirs) const
	{
		OGRE_LOCK_AUTO_MUTEX
        FileInfoListPtr ret = FileInfoListPtr(OGRE_NEW_T(FileInfoList, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);

        FileInfoList::const_iterator i, iend;
        iend = mFileList.end();
        for (i = mFileList.begin(); i != iend; ++i)
            if ((dirs == (i->compressedSize == size_t (-1))))
                // Check name matches pattern (zip is case insensitive)
                if (StringUtil::match(i->basename, pattern, false))
				{
                    ret->push_back(*i);
					ret->back().filename = ret->back().basename;
					ret->back().path = "";
				}
		
        LogManager::getSingleton().logMessage("ZZIP: get file info" +pattern);
        return ret;
	}
	bool AndroidArchive::exists(const String& filename)
	{
		return ZipArchive::exists(assetsDirectory + filename);
	}
	//////////////////////////////////////////////////////////////////////////////

	const String &AndroidArchiveFactory::getType() const
	{
		static String type = "Android";
		return type;
	}

}