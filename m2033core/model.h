#ifndef __M2033_MODEL_H__
#define __M2033_MODEL_H__

#include "prerequisites.h"
#include "mesh.h"
#include "skeleton.h"

namespace m2033
{
	class model
	{
	public:
		typedef std::vector<mesh> meshes;

		inline model();
		inline ~model();

		inline void add_mesh( const mesh &m );
		inline void set_skeleton( const skeleton &s );
		inline void set_type( int type );

		inline mesh get_mesh( unsigned idx ) const;
		inline unsigned get_num_meshes() const;
		inline skeleton get_skeleton() const;
		inline int get_type() const;

	private:
		meshes		meshes_;
		skeleton	skeleton_;

		int			type_;
	};

	inline model::model() : type_(-1) {}
	inline model::~model() {}

	inline void model::add_mesh( const mesh &m ) { meshes_.push_back( m ); }
	inline void model::set_skeleton( const skeleton &s ) { skeleton_ = s; }
	inline void model::set_type( int type ) { type_ = type; }

	inline mesh model::get_mesh( unsigned idx ) const { assert( idx < meshes_.size() ); return meshes_[idx]; }
	inline unsigned model::get_num_meshes() const { return meshes_.size(); }
	inline skeleton model::get_skeleton() const { return skeleton_; }
	inline int model::get_type() const { return type_; }
}

#endif // __M2033_MODEL_H__