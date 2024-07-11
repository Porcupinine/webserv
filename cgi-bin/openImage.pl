#!/usr/bin/perl

use strict;
use warnings;
use CGI;

# Create a new CGI object
my $cgi = CGI->new;

# Set values
my $upload_dir = $ENV{'UPLOAD_DIR'} // die "UPLOAD_DIR not set";
my $query_string = $ENV{'QUERY_STRING'} // die "QUERY_STRING not set";
my $file_path = "$upload_dir/$query_string";

# Determine the content type based on the file extension
my $mime = MIME::Types->new;
my $content_type = $mime->mimeTypeOf($file_path) || 'application/octet-stream';

# Check if the file file exists
if (-e $file_path) {
    # Open the file
    open(my $file, '<', $file_path) or die "Cannot open file: $!";
    binmode $file; # Binmode for binary file

    # Get the file size
    my $file_size = -s $file;

    # Read the image file content
    my $file_data;
    read($file, $file_data, $file_size);
    close($file);

    # Send the content type header and image data to the browser
    print $cgi->header(-type => $content_type, -Content_Length => $file_size);
    print $file_data;
} else {
    # Image file not found, send an error message
    print $cgi->header('text/plain');
    print "Image file not found.";
}
