=pod

=head1 NAME

Metro2033 - some internal data from Metro2033

=head1 DESCRIPTION

Perl version forked off an original C++ code 
http://code.google.com/p/metro2033-tools/
by Ivan Shishkin

=head1 AUTHOR

dmitry@karasik.eu.org

=cut

package Metro2033;

die "unsupported float on this platform" unless 42.0 == unpack('f', "\x00\x00\x28\x42");

package Metro2033::Reader;

use Moose;

has chunks => (
    is      => 'rw',
    isa     => 'ArrayRef',
    default => sub { [] },
);

has ptr => (
    is      => 'rw',
    isa     => 'Int',
    default => 0,
);

has data => (
    is      => 'rw',
);

sub read
{
    my ($class, $file) = @_;
    open F, '<', $file or die "cannot open $file:$!";
    local $/;
    binmode F;
    my $level = <F>;
    close F;
    return $class->new( data => $level );
}

sub bin
{
    my ( $self, $offset ) = @_;
    return '' unless $offset;
    warn "read past end: $self->{ptr}" if $self->{ptr} + $offset > $self->end;
    my $r = substr($self->{data}, $self->{ptr}, $offset);
    $self->{ptr} += $offset;
    return $r;
}   

sub _read
{
    my ( $self, $unpack, $offset, $howmany ) = @_;
    $howmany //= 1;
    $offset *= $howmany;
    warn "read past end: $self->{ptr}" if $self->{ptr} + $offset > $self->end;
    my @r = unpack($unpack.$howmany, substr($self->{data}, $self->{ptr}, $offset));
    $self->{ptr} += $offset;
    die "reading more than one in scalar context" if !wantarray && $howmany > 1;
    return wantarray ? @r : $r[0];
}

sub u32 { shift->_read('V', 4, shift) }
sub u16 { shift->_read('v', 2, shift) }
sub f32 { shift->_read('f', 4, shift) }
sub chunk { substr($_[0]->{data}, $_[0]->begin, $_[0]->size) }

sub string
{
    my $self = shift;
    pos($self->{data}) = $self->{ptr};
    if ( $self->{data} =~ m/\G(.*?)\0/gc ) {
        $self->ptr(pos($self->{data}));
        return $1;
    } else {
        warn "read str past chunk end: $self->{ptr}";
        $self->ptr($self->end);
        return undef;
    }
}

sub open_chunk
{
    my ($self, $xid) = @_;
    return $self->open_chunk_id($xid) if defined $xid;

    my ($id, $sz) = ($self->u32, $self->u32);
    push @{$self->chunks}, {
        begin => $self->ptr,
        end   => $self->ptr + $sz,
        id    => $id & 0x7FFFFFFF,
    };
#    warn "open chunk: $id $sz ", 4*$self->{ptr}, "\n";
    return 1;
}

sub close_chunk
{
    my $self = shift;
    if ( my $c = pop @{ $self->{chunks} } ) {
        $self->ptr($c->{end});
    }
}

sub open_chunk_id
{
    my ( $self, $id ) = @_;
    $self->seek(0);
    while ( $self->elapsed ) {
        $self->open_chunk;
        return 1 if $self->id == $id;
        $self->close_chunk;
    }
    return 0;
}

sub begin   { @{ $_[0]->{chunks} } ? $_[0]->{chunks}->[-1]->{begin} : 0 }
sub end     { @{ $_[0]->{chunks} } ? $_[0]->{chunks}->[-1]->{end} : length $_[0]->{data} }
sub id      { @{ $_[0]->{chunks} } ? $_[0]->{chunks}->[-1]->{id} : -1 }
sub size    { $_[0]->end - $_[0]->begin }
sub elapsed { ($_[0]->end - $_[0]->ptr) > 0 }
sub tell    { $_[0]->ptr - $_[0]->begin }
sub advance { $_[0]->seek( $_[0]->tell + $_[1] ) }

sub seek
{
    my ( $self, $pos) = @_;
    die "seek: too far" unless $self->begin + $pos <= $self->end;
    $self->ptr($self->begin + $pos);
}


package Metro2033::Level;

use Moose;

use constant {
    UNUSED_CHUNK_ID => 0x01,
    TEXTURE_NAME_CHUNK_ID => 0x02,
    STATIC_VERTEX_CHUNK_ID => 0x03,
    DYNAMIC_VERTEX_CHUNK_ID => 0x05,
    MODEL_CHUNK_ID => 0x09,
    BONES_CHUNK_ID => 0x0D,
    MESH_NAMES_CHUNK_ID => 0x10,
    SKELETON_NAME_CHUNK_ID => 0x14,
    LEVEL_PARTS_CHUNK_ID => 0x03,
    LEVEL_TEXTURES_CHUNK_ID => 0x02,
    LEVEL_VB_CHUNK_ID => 0x09,
    LEVEL_IB_CHUNK_ID => 0x0A,
};

sub load
{
    my ($class, $geom_file) = @_;

    my $level_file = $geom_file;
    $level_file =~ s/\.[^.]*$//;

    my $r = Metro2033::Reader->read($level_file);

    my @parts;
    if( $r->open_chunk( LEVEL_PARTS_CHUNK_ID ) ) {
        while( $r->elapsed ) {
            $r->open_chunk();
            if( $r->open_chunk( 0x15 ) ) {
                $r->advance( 4 );
                push @parts, {
                    vb_offset => $r->u32,
                    vb_size   => $r->u32,
                    ib_offset => $r->u32,
                    ib_size   => $r->u32,
                };
                $r->close_chunk();
                $r->open_chunk( 0x01 );
                $r->advance( 2 );
                $parts[-1]->{texture_id} = $r->u16;
                $r->close_chunk();
            }
            $r->close_chunk();
        }
        $r->close_chunk();
    }

    my @textures;
    if( $r->open_chunk( LEVEL_TEXTURES_CHUNK_ID ) ) {
        my $k = $r->u16();
        for( my $i = 0; $i < $k; $i++ ) {
            $r->string;
            push @textures, $r->string;
            $r->string;
            $r->advance( 4 );
        }
        $r->close_chunk();
    }

    my $s = Metro2033::Reader->read($geom_file);

    my @vbuffers;
    if( $s->open_chunk( LEVEL_VB_CHUNK_ID ) ) {
        for( @parts) {
            $s->seek( $_->{vb_offset} * 32 );
            push @vbuffers, $s->ptr;
        }
        $s->close_chunk();
    }

    my @ibuffers;
    if( $s->open_chunk( LEVEL_IB_CHUNK_ID ) ) {
        for( @parts ) {
            $s->seek( $_->{ib_offset} * 2 );
            push @ibuffers, $s->ptr;
        }
        $s->close_chunk();
    }

    my @geom;
    for( my $i = 0; $i < @parts; $i++ ) {
        my $m = Metro2033::Mesh-> new(
            texture => 'textures/' . $textures[ $parts[$i]->{texture_id} ],
            name    => "level_$i",
        );
        $m->init_level(
            $s,
            $vbuffers[$i],
            $parts[$i]->{vb_size},
            $ibuffers[$i],
            $parts[$i]->{ib_size},
        );
        push @geom, $m;
    };

    return @geom;
}

package Metro2033::Mesh;

use Moose;

has vertices  => ( is => 'rw', isa => 'ArrayRef', default => sub { [] } );
has indices   => ( is => 'rw', isa => 'ArrayRef', default => sub { [] } );
has normals   => ( is => 'rw', isa => 'ArrayRef', default => sub { [] } );
has texcoords => ( is => 'rw', isa => 'ArrayRef', default => sub { [] } );

has name    => ( is => 'rw', isa => 'Str' );
has texture => ( is => 'rw', isa => 'Str' );

# struct level_geom_vertex
# {
#     float x, y, z;         4 * 3
#     unsigned normal;       4
#   unsigned tangent;        4
#   unsigned binormal;       4
#     short u, v;            4
#   unsigned unused; // ?    4
# }; = 32

sub init_level
{
    my ( $self, $reader, $vertices, $num_vertices, $indices, $num_indices) = @_;

    my ($V, $T, $N, $I) = ($self->vertices, $self->texcoords, $self->normals, $self->indices);
    for( my $i = 0; $i < $num_vertices; $i++ ) {
        my ( $x, $y, $z, $normal, $tangent, $binomial, $u, $v, $unused ) = unpack('fffVVVssV', substr( $reader->{data}, $vertices + $i * 32, 32 ));
        push @$V, [ -$x, -$z, $y ];
        push @$N, [ 
            (($normal<<16)&0xff)/255.0, 
            (($normal<<8)&0xff)/255.0, 
            ($normal&0xff)/255.0, 
        ];
        push @$T, [ $u / 1024.0, $v / 1024.0 ];
    }

    my @i = unpack('S*', substr( $reader->{data}, $indices, $num_indices * 2 ));
    for (my $i = 0; $i < @i; $i+=3) {
        push @$I, [@i[$i+2,$i+1,$i]];
    }
}

package Metro2033::Portal;

use Moose;

has name     => (is => 'rw', isa => 'Str');
has vertices => (is => 'rw', isa => 'ArrayRef', default => sub { [] } );

sub load
{
    my ($class, $file) = @_;

    my $r = Metro2033::Reader->read($file);

    my @parts;
    while ($r->elapsed) {
        $r->open_chunk;
        if ( $r->id < 65500 ) {
            my $n = $r->u32;
            my @v; 
            for ( my $i = 0; $i < $n; $i++) {
                my ( $x, $y, $z ) = $r->f32(3);
                push @v, [ -$x, -$z, $y ];
            }
            my $sz = $r->string;
            my $obj = $class->new(
                name => $sz,
                vertices => \@v,
            );
            push @parts, $obj;
        }
        $r->close_chunk;
    }
    return @parts;
}

1;
