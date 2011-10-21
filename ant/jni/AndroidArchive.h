#ifndef __ADPArchive_H__
#define __ADPArchive_H__

#include <OgreArchive.h>
#include <OgreZip.h>
#include <OgreArchiveFactory.h>

#include <zzip/zzip.h>
#include <zzip/plugin.h>
#include <jni.h>

namespace Ogre{	

	class AndroidArchive : public ZipArchive
	{
	private:
		JNIEnv					*mEnv;
		const std::string		apk_name;
		zzip_plugin_io_handlers pluginIo;
	public:
		AndroidArchive(const std::string& archiveName,const String& name, const String& archType, JNIEnv *env);
        ~AndroidArchive();

        /// @copydoc Archive::load
        virtual void load();
        /// @copydoc Archive::unload
        virtual void unload();
		
		virtual StringVectorPtr list(bool recursive, bool dirs);
		virtual FileInfoListPtr listFileInfo(bool recursive, bool dirs);
		virtual DataStreamPtr open(const String& filename, bool readOnly) const;
		virtual StringVectorPtr find(const String& pattern, bool recursive, bool dirs);
		virtual FileInfoListPtr findFileInfo(const String& pattern, bool recursive, bool dirs) const;
		virtual bool exists(const String& filename);
	};
	
	class AndroidArchiveFactory : public ArchiveFactory
    {
	private:
		JNIEnv	*mEnv;
		std::string apk_name;
    public:
		AndroidArchiveFactory(JNIEnv *env,const std::string& archiveName):mEnv(env),apk_name(archiveName){}
        virtual ~AndroidArchiveFactory() {}
        /// @copydoc FactoryObj::getType
        const String& getType(void) const;
        /// @copydoc FactoryObj::createInstance
        Archive *createInstance( const String& name ) 
        {
            return OGRE_NEW AndroidArchive(apk_name,name, getType(), mEnv);
        }
        /// @copydoc FactoryObj::destroyInstance
        void destroyInstance( Archive* arch) { OGRE_DELETE arch; }
    };

	/*
	class AndroidArchive : public Archive
	{
	private:
		JNIEnv *mEnv;
		void *mBuffer;
		size_t mBufferSize;
		ACPFile mFile;
	public:
		AndroidArchive(const String& name, const String& archType, JNIEnv *env);
        ~AndroidArchive();

        /// @copydoc Archive::isCaseSensitive
        bool isCaseSensitive(void) const;

        /// @copydoc Archive::load
        void load();
        /// @copydoc Archive::unload
        void unload();

        /// @copydoc Archive::open
        DataStreamPtr open(const String& filename, bool readOnly = true) const;

		/// @copydoc Archive::create
		DataStreamPtr create(const String& filename) const;

		/// @copydoc Archive::delete
		void remove(const String& filename) const;

		/// @copydoc Archive::list
        StringVectorPtr list(bool recursive = true, bool dirs = false);

        /// @copydoc Archive::listFileInfo
        FileInfoListPtr listFileInfo(bool recursive = true, bool dirs = false);

        /// @copydoc Archive::find
        StringVectorPtr find(const String& pattern, bool recursive = true,
            bool dirs = false);

        /// @copydoc Archive::findFileInfo
        FileInfoListPtr findFileInfo(const String& pattern, bool recursive = true,
            bool dirs = false) const;

        /// @copydoc Archive::exists
        bool exists(const String& filename);

		/// @copydoc Archive::getModifiedTime
		time_t getModifiedTime(const String& filename);
	};
	
	class AndroidArchiveFactory : public ArchiveFactory
    {
	private:
	JNIEnv *mEnv;
    public:
		AndroidArchiveFactory(JNIEnv *env):mEnv(env){}
        virtual ~AndroidArchiveFactory() {}
        /// @copydoc FactoryObj::getType
        const String& getType(void) const;
        /// @copydoc FactoryObj::createInstance
        Archive *createInstance( const String& name ) 
        {
            return OGRE_NEW AndroidArchive(name, getType(), mEnv);
        }
        /// @copydoc FactoryObj::destroyInstance
        void destroyInstance( Archive* arch) { OGRE_DELETE arch; }
    };*/
}

#endif