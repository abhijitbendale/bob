#ifndef PGM_IMAGE_FILE_INC
#define PGM_IMAGE_FILE_INC

#include "ip/ImageFile.h"

namespace Torch {

	/** This class is designed to handle a PGM image on the disk

	    @author Sebastien Marcel (marcel@idiap.ch)
	    @version 2.0
	    \date
	    @since 1.0
	*/
	class pgmImageFile : public ImageFile
	{
	public:
		/**@name constructor and destructor */
		//@{
		/** Makes a ImageFile.
		*/
		pgmImageFile();

		/// Destructor
		virtual ~pgmImageFile();
		//@}

	protected:

		/**@name read/write image header and image pixmap */
		//@{
		/** read the image header from the file.
		*/
		virtual bool 		readHeader(Image& image);

		/// read the image pixmap from the file
		virtual bool 		readPixmap(Image& image);

		/** write the image header to a file.
		*/
		virtual bool 		writeHeader(const Image& image);

		/// write the #pixmap_# into the file
		virtual bool 		writePixmap(const Image& pixmap);
		//@}
	};
}

#endif