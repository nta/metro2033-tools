#ifndef __M2033_SKELETON_H__
#define __M2033_SKELETON_H__

#include "prerequisites.h"
#include "math.h"

namespace m2033
{
	class skeleton
	{
	public:
		struct bone
		{
			std::string name;
			std::string parent;
			vec3 pos;
			vec3 rot;
		};

		inline skeleton();
		inline ~skeleton();

		inline void add_bone( const std::string &name,
			const std::string &parent, const vec3 &pos, const vec3 &rot );
		inline void add_bone( const bone &b );

		inline bone get_bone( unsigned idx ) const;
		inline unsigned get_num_bones() const;

		inline void clear();

	private:
		typedef std::vector<bone> bones;

		bones	bones_;
	};

	inline skeleton::skeleton() {}
	inline skeleton::~skeleton() {}

	inline void skeleton::add_bone( const std::string &name,
			const std::string &parent, const vec3 &pos, const vec3 &rot )
	{
		bone b;

		b.name = name;
		b.parent = parent;
		b.pos = pos;
		b.rot = rot;

		add_bone( b );
	}

	inline void skeleton::add_bone( const skeleton::bone &b ) { bones_.push_back( b ); }

	inline skeleton::bone skeleton::get_bone( unsigned idx ) const { assert( idx < bones_.size() ); return bones_[idx]; }
	inline unsigned skeleton::get_num_bones() const { return bones_.size(); }

	inline void skeleton::clear() { bones_.clear(); }
}

#endif // __M2033_SKELETON_H__