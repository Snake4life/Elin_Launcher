#!/usr/bin/perl
use File::stat;

#@files = array();
@dir = @ARGV;
sub dirlist
{
  $arg = $_[0];
  opendir($dh, $arg);
  while (readdir $dh) {
    if ($_  ne "." && $_ ne "..") {
      $fln = $arg."/".$_;
      if (-d $fln) {
        push(@dir, $fln);
      } else {
        ($md5) = split(" ", `md5sum -b "$fln"`);
        $size = -s $fln;
#       $fln =~ s/([^A-Za-z0-9\+-_])/sprintf("%%%02X", ord($1))/seg;
#       $fln = utf8::unicode_to_native($fln);
        printf "$md5\t$size\t$fln\n";
      }
    }
  }
  close $dh;
}

while (@dir)
{
  dirlist($dir[0]);
  shift(@dir);
}

