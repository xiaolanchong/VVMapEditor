#ifndef _VVMSETTINGS_H_INCLUDED__
#define _VVMSETTINGS_H_INCLUDED__


#include "VVMErrors.h"
/*
VVMINT_EDITOR		1
VVMINT_COMMUTATOR	2
VVMINT_CLIENT		3
VVMINT_MASTER		4
*/
	struct VVMInfo
	{
		long who_is;
		bool save_images;
		bool load_images;

		VVMInfo(long _who_is) :who_is (_who_is) {}

		bool can_edit() const { return (who_is == VVMINT_EDITOR) || (who_is == VVMINT_COMMUTATOR);}
		bool is_editor() const { return (who_is == VVMINT_EDITOR);}
		bool is_commutator() const { return (who_is == VVMINT_COMMUTATOR);}
		bool is_client() const { return (who_is == VVMINT_EDITOR);}
	};

#endif