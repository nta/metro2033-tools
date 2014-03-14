#!perl
use strict;
use warnings;
use FindBin qw($Bin);
use lib $Bin;
use Metro2033;
use Prima qw(Application Buttons MsgBox GLWidget);
use OpenGL qw(:glfunctions :glconstants :glutfunctions :glutconstants);

sub usage
{
    print <<USAGE;
view metro 2033 level

format: perl level_view.pl PATH , 
f.ex. "C:/m2033/data/content/maps/l05_lost_tunnel where level data 
(i.e. level.bin. level.geom_pc etc files are unpacked).

keys to navigate:
    arrows, mouse drag  - movement
    mouse wheel and +/- - zoom
    w,s,a,d,q,z         - rotates

by dmitry\@karasik.eu.org

USAGE
    exit 1;
}

usage unless @ARGV;
my @meshes  = Metro2033::Level->load($ARGV[0] . '/level.geom_pc');
my @portals = Metro2033::Portal->load($ARGV[0] . '/level.portals');

my $top = Prima::MainWindow-> new(
	size => [ 300, 300 ],
	text => 'Metro 2033 level view',
);
	

my $mod;
my @t  = (0,0,1);
my @a  = (0,0,0);

glutInit;
	
my $gl = $top-> insert( 'Prima::GLWidget' => 
	pack	  => { expand => 1, fill => 'both'},
	selectable => 1,
	gl_config => { double_buffer => 1 },
	onCreate  => sub {
		shift-> gl_select;
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-300, 300, -300, 300, -1000.0, 4000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		$mod = glGenLists(1); 
		glNewList($mod, GL_COMPILE);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glColor3f(1,1,1);
		for ( @meshes ) {
			my ($i, $v) = ($_->indices, $_->vertices);
			for ( @$i ) {
				glBegin(GL_LINE_LOOP);
				glVertex3f( @{$v->[$_]} ) for @$_;
				glEnd();
			}
		}

		glColor3f(1,0,0);
        glLineWidth(3);
		for ( @portals ) {
			glBegin(GL_LINE_LOOP);
			glVertex3f( @$_ ) for @{$_->vertices};
			glEnd();
			glRasterPos3f( @{$_->vertices->[0]});
			#glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, $_->name);
		}
		glPopAttrib();
		glEndList();
	},
	onPaint      => sub {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glTranslatef($t[0],$t[1],0);
		glScalef(($t[2])x 3);
		glRotatef($a[0], 1,0,0);
		glRotatef($a[1], 0,1,0);
		glRotatef($a[2], 0,0,1);
		glCallList($mod);
		glPopMatrix();
	},
    	onKeyDown => sub {
		my ( $self, $code, $key, $mod ) = @_;

		   if ( chr($code) eq 'w' ) { $a[0] += 5; }
		elsif ( chr($code) eq 's' ) { $a[0] -= 5; }
		elsif ( chr($code) eq 'a' ) { $a[1] += 5; }
		elsif ( chr($code) eq 'd')  { $a[1] -= 5; }
		elsif ( chr($code) eq 'q' ) { $a[2] += 5; }
		elsif ( chr($code) eq 'z' ) { $a[2] -= 5; }
		elsif ( chr($code) eq '+' ) { $t[2] *= 1.05; }
		elsif ( chr($code) eq '-' ) { $t[2] /= 1.05; }
		elsif ( $key == kb::Up )    { $t[1] -= 5; }
		elsif ( $key == kb::Down )  { $t[1] += 5; }
		elsif ( $key == kb::Left )  { $t[0] += 5; }
		elsif ( $key == kb::Right ) { $t[0] -= 5; }
		$self->repaint;
	},
	onMouseWheel => sub {
		my ($self, $mod, $x, $y, $z) = @_;
		$t[2] *= ($z > 0) ? 1.05 :0.95;
		$self->repaint;
	},
	onMouseDown => sub {
		my ( $self, $btn, $mod, $x, $y) = @_;
		return if $self-> {drag};
		$self-> {drag}=1;
		$self-> {anchor} = [$x, $y];
		$self-> {old}   = [@t];
		$self-> capture(1);
	},
	onMouseUp => sub {
		my ( $self, $btn, $mod, $x, $y) = @_;
		return unless $self-> {drag};
		$self-> {drag}=0;
		$self-> capture(0);
	},
	onMouseMove => sub {
		my ( $self, $mod, $x, $y) = @_;
		return unless $self-> {drag};
		$t[0] = $self-> {old}->[0] + $x - $self->{anchor}->[0];
		$t[1] = $self-> {old}->[1] + $y - $self->{anchor}->[1];
		$self->repaint;
	}

);
	
run Prima;
