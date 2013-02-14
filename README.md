<html>
<head>
<title><b>stylom - A command line utility for stylometric text analysis</b></title>
</head>
<body>
<div id="content">

<h1 class="title"><b>stylom - A command line utility for stylometric text analysis</b></h1>


<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1 <span style="text-decoration:underline;">Description</span> </a></li>
<li><a href="#sec-2">2 <span style="text-decoration:underline;">Installation</span> </a></li>
<li><a href="#sec-3">3 <span style="text-decoration:underline;">Generating Author Fingerprints</span> </a></li>
<li><a href="#sec-4">4 <span style="text-decoration:underline;">Matching Unknown Text</span> </a></li>
<li><a href="#sec-5">5 <span style="text-decoration:underline;">Counter-Stylometry</span> </a></li>
<li><a href="#sec-6">6 <span style="text-decoration:underline;">Plotting Text Distributions</span> </a></li>
<li><a href="#sec-7">7 <span style="text-decoration:underline;">Options</span> </a>
<ul>
<li><a href="#sec-7.1">7.1 -t,&ndash;text &lt;string or filename&gt; </a></li>
<li><a href="#sec-7.2">7.2 -n,&ndash;name &lt;author&gt; </a></li>
<li><a href="#sec-7.3">7.3 -f,&ndash;fingerprint </a></li>
<li><a href="#sec-7.4">7.4 -m,&ndash;match &lt;directory&gt; </a></li>
<li><a href="#sec-7.5">7.5 -l,&ndash;list &lt;directory&gt; </a></li>
<li><a href="#sec-7.6">7.6 -c,&ndash;compare &lt;fingerprint&gt; </a></li>
<li><a href="#sec-7.7">7.7 &ndash;missing &lt;fingerprint&gt; </a></li>
<li><a href="#sec-7.8">7.8 &ndash;more &lt;fingerprint&gt; </a></li>
<li><a href="#sec-7.9">7.9 &ndash;less &lt;fingerprint&gt; </a></li>
<li><a href="#sec-7.10">7.10 -p,&ndash;plot &lt;directory&gt; </a></li>
<li><a href="#sec-7.11">7.11 &ndash;plottexts &lt;directory&gt; </a></li>
<li><a href="#sec-7.12">7.12 &ndash;title &lt;text&gt; </a></li>
<li><a href="#sec-7.13">7.13 -v,&ndash;version </a></li>
<li><a href="#sec-7.14">7.14 -h,&ndash;help </a></li>
</ul>
</li>
<li><a href="#sec-8">8 <span style="text-decoration:underline;">Bugs</span> </a></li>
<li><a href="#sec-9">9 <span style="text-decoration:underline;">Author</span> </a></li>
</ul>
</div>
</div>

<div id="outline-container-1" class="outline-2">
<h2 id="sec-1"><span class="section-number-2">1</span> <span style="text-decoration:underline;">Description</span> </h2>
<div class="outline-text-2" id="text-1">


<p>
This is a tool to facilitate the stylometric analysis of texts.  It could be used for academic disambiguation of disputed authorship, and to help identify plagiarists, astroturfers, sockpuppets and guerilla marketers.  Another possible use case is as an assistance to the anonymisation of writing style.
</p>
<p>
Every author has their own unique writing style, and if enough writing examples are available then it's possible to construct a quantitative model of their style which can be compared against others.
</p>

</div>

</div>

<div id="outline-container-2" class="outline-2">
<h2 id="sec-2"><span class="section-number-2">2</span> <span style="text-decoration:underline;">Installation</span> </h2>
<div class="outline-text-2" id="text-2">


<p>
This program is easy to compile and install as follows:
</p>



<pre class="example">sudo make install
</pre>




<p>
If you wish to generate a Debian package you can also run the following script:
</p>



<pre class="example">./debian.sh
</pre>




<p>
To plot graphs you will need to have gnuplot installed. For example:
</p>



<pre class="example">sudo apt-get install gnuplot
</pre>





</div>

</div>

<div id="outline-container-3" class="outline-2">
<h2 id="sec-3"><span class="section-number-2">3</span> <span style="text-decoration:underline;">Generating Author Fingerprints</span> </h2>
<div class="outline-text-2" id="text-3">


<p>
Fingerprints are high dimensional vectors which represent the writing style of particular authors.  To generate a fingerprint first gather some examples of the author's writing within plain text files and put them in a directory.  Then run something similar to the following:
</p>



<pre class="example">cat texts/dickens/* | stylom -n "Charles Dickens" -f &gt; fingerprints/dickens.style
</pre>




<p>
Here the texts get piped into the command and the resulting fingerprint is then saved to a file.  Ideally the amount of example text should be as large as possible.
</p>

</div>

</div>

<div id="outline-container-4" class="outline-2">
<h2 id="sec-4"><span class="section-number-2">4</span> <span style="text-decoration:underline;">Matching Unknown Text</span> </h2>
<div class="outline-text-2" id="text-4">


<p>
If you have a file containing a sample of text for which the author is unknown, but who is likely to be an author for which you have previously generated a fingerprint, you can find the most likely candidate in the following way:
</p>



<pre class="example">cat unknown.txt | stylom --match fingerprints
</pre>




<p>
Where <i>fingerprints</i> is the name of a directory containing previously saved fingerprints for a variety of possible authors.  This will return a single name, but it's also possible to return a list of candidates in the following way.
</p>



<pre class="example">cat unknown.txt | stylom --list fingerprints
</pre>





</div>

</div>

<div id="outline-container-5" class="outline-2">
<h2 id="sec-5"><span class="section-number-2">5</span> <span style="text-decoration:underline;">Counter-Stylometry</span> </h2>
<div class="outline-text-2" id="text-5">


<p>
Stylometrics have various legitimate uses in terms of the study of disputed historical texts or detecting plagiarism.  However, a possible danger of stylometric methods is their abuse by powerful organisations against bloggers or whistleblowers who may be trying to raise issues of public concern in an anonymous manner in order to avoid serious reprisals.
</p>
<p>
A known method to render stylometry less effective in determining the identity of an author is to try to make the style of writing as similar as possible to some existing author, hence obscuring the differences.  Automatically transforming the writing style of a given text into the writing style of a known author is a very difficult and likely AI-complete problem, but this program can be used provide the writer with advice on how to alter their work to achieve a greater degree of similarity.
</p>



<pre class="example">cat mytext.txt | stylom -c fingerprints/charles_dickens.style
</pre>




<p>
or
</p>



<pre class="example">stylom -t mytext.txt -c fingerprints/charles_dickens.style
</pre>




<p>
Takes some text and compares it to a pre-computed fingerprint for Charles Dickens.  It then gives some indication of how to alter <i>mytext.txt</i> in order to make it more similar to Dickens' writing style.  The number of differences is also shown, which can be used as an indicator of progress.
</p>
<p>
If you are only interested in which words are present in <i>mytext.txt</i> but missing from Dickens writing:
</p>



<pre class="example">cat mytext.txt | stylom --missing fingerprints/charles_dickens.style
</pre>




<p>
or
</p>



<pre class="example">stylom -t mytext.txt --missing fingerprints/charles_dickens.style
</pre>




<p>
The results could then be used by some other program (maybe highlighted in a text editor GUI).  The same can also be done for words which are more frequent within the fingerprint or less frequent within the fingerprint.
</p>



<pre class="example">cat mytext.txt | stylom --more fingerprints/charles_dickens.style
</pre>




<p>
or
</p>



<pre class="example">cat mytext.txt | stylom --less fingerprints/charles_dickens.style
</pre>




<p>
The above methods can be used to make the vocabulary and the word frequency similar to an existing known author.  It's not a perfect solution, since it takes no account of syntax, and it still involves mannual editing effort by the writer.
</p>

</div>

</div>

<div id="outline-container-6" class="outline-2">
<h2 id="sec-6"><span class="section-number-2">6</span> <span style="text-decoration:underline;">Plotting Text Distributions</span> </h2>
<div class="outline-text-2" id="text-6">


<p>
You can plot the distribution of fingerprints in the following way.
</p>



<pre class="example">stylom --plot &lt;directory&gt;
</pre>




<p>
This plots any fingerprints within the given directory within a 2D graph so that you can see how authors are distributed.  The graph is saved by default with the filename <i>result.png</i>
</p>
<p>
If necessary you can also specify a title for the graph.
</p>



<pre class="example">stylom --title "My Graph Title" --plot &lt;directory&gt;
</pre>




<p>
You can also plot texts more directly without having previously calculated fingerprints for them.
</p>



<pre class="example">stylom --title "My Texts" --plottexts &lt;directory&gt;
</pre>





</div>

</div>

<div id="outline-container-7" class="outline-2">
<h2 id="sec-7"><span class="section-number-2">7</span> <span style="text-decoration:underline;">Options</span> </h2>
<div class="outline-text-2" id="text-7">



</div>

<div id="outline-container-7.1" class="outline-3">
<h3 id="sec-7.1"><span class="section-number-3">7.1</span> -t,&ndash;text &lt;string or filename&gt; </h3>
<div class="outline-text-3" id="text-7.1">

<p>This can be either text to be analysed or a filename containing plain text.
</p>
</div>

</div>

<div id="outline-container-7.2" class="outline-3">
<h3 id="sec-7.2"><span class="section-number-3">7.2</span> -n,&ndash;name &lt;author&gt; </h3>
<div class="outline-text-3" id="text-7.2">

<p>The name of the author.
</p>
</div>

</div>

<div id="outline-container-7.3" class="outline-3">
<h3 id="sec-7.3"><span class="section-number-3">7.3</span> -f,&ndash;fingerprint </h3>
<div class="outline-text-3" id="text-7.3">

<p>Displays a fingerprint on the console in XML format.
</p>
</div>

</div>

<div id="outline-container-7.4" class="outline-3">
<h3 id="sec-7.4"><span class="section-number-3">7.4</span> -m,&ndash;match &lt;directory&gt; </h3>
<div class="outline-text-3" id="text-7.4">

<p>Match the given fingerprint against ones stored within a directory.
</p>
</div>

</div>

<div id="outline-container-7.5" class="outline-3">
<h3 id="sec-7.5"><span class="section-number-3">7.5</span> -l,&ndash;list &lt;directory&gt; </h3>
<div class="outline-text-3" id="text-7.5">

<p>Match the given fingerprint against ones stored within a directory and show a list of the most similar authors.
</p>
</div>

</div>

<div id="outline-container-7.6" class="outline-3">
<h3 id="sec-7.6"><span class="section-number-3">7.6</span> -c,&ndash;compare &lt;fingerprint&gt; </h3>
<div class="outline-text-3" id="text-7.6">

<p>Compares the current text against the given fingerprint and reports differences.
</p>
</div>

</div>

<div id="outline-container-7.7" class="outline-3">
<h3 id="sec-7.7"><span class="section-number-3">7.7</span> &ndash;missing &lt;fingerprint&gt; </h3>
<div class="outline-text-3" id="text-7.7">

<p>Report words which are present in the current text but which are missing from the given fingerprint.
</p>
</div>

</div>

<div id="outline-container-7.8" class="outline-3">
<h3 id="sec-7.8"><span class="section-number-3">7.8</span> &ndash;more &lt;fingerprint&gt; </h3>
<div class="outline-text-3" id="text-7.8">

<p>Report words which are present more frequently in the given fingerprint than in the current text.
</p>
</div>

</div>

<div id="outline-container-7.9" class="outline-3">
<h3 id="sec-7.9"><span class="section-number-3">7.9</span> &ndash;less &lt;fingerprint&gt; </h3>
<div class="outline-text-3" id="text-7.9">

<p>Report words which are present less frequently in the given fingerprint than in the current text.
</p>
</div>

</div>

<div id="outline-container-7.10" class="outline-3">
<h3 id="sec-7.10"><span class="section-number-3">7.10</span> -p,&ndash;plot &lt;directory&gt; </h3>
<div class="outline-text-3" id="text-7.10">

<p>Plots fingerprints within the given directory using gnuplot.  The resulting image is saved as <i>result.png</i>
</p>
</div>

</div>

<div id="outline-container-7.11" class="outline-3">
<h3 id="sec-7.11"><span class="section-number-3">7.11</span> &ndash;plottexts &lt;directory&gt; </h3>
<div class="outline-text-3" id="text-7.11">

<p>Plots texts within the given directory using gnuplot.  The resulting image is saved as <i>result.png</i>
</p>
</div>

</div>

<div id="outline-container-7.12" class="outline-3">
<h3 id="sec-7.12"><span class="section-number-3">7.12</span> &ndash;title &lt;text&gt; </h3>
<div class="outline-text-3" id="text-7.12">

<p>Specify a title for use together with the &ndash;plot option.
</p>
</div>

</div>

<div id="outline-container-7.13" class="outline-3">
<h3 id="sec-7.13"><span class="section-number-3">7.13</span> -v,&ndash;version </h3>
<div class="outline-text-3" id="text-7.13">

<p>Show the version number.
</p>
</div>

</div>

<div id="outline-container-7.14" class="outline-3">
<h3 id="sec-7.14"><span class="section-number-3">7.14</span> -h,&ndash;help </h3>
<div class="outline-text-3" id="text-7.14">

<p>Show help.
</p>

</div>
</div>

</div>

<div id="outline-container-8" class="outline-2">
<h2 id="sec-8"><span class="section-number-2">8</span> <span style="text-decoration:underline;">Bugs</span> </h2>
<div class="outline-text-2" id="text-8">


<p>
Report all bugs to /<a href="https://bugs.launchpad.net/stylom/">https://bugs.launchpad.net/stylom/</a>
</p>

</div>

</div>

<div id="outline-container-9" class="outline-2">
<h2 id="sec-9"><span class="section-number-2">9</span> <span style="text-decoration:underline;">Author</span> </h2>
<div class="outline-text-2" id="text-9">


<p>
Bob Mottram &lt;bob@sluggish.dyndns.org&gt;
</p>
<p>
GPG ID: 0xEA982E38
</p>
<p>
GPG Fingerprint: D538 1159 CD7A 2F80 2F06  ABA0 0452 CC7C EA98 2E38
</p>
<p>
<a href="http://sluggish.dyndns.org">http://sluggish.dyndns.org</a>
</p></div>
</div>
<div id="postamble">
<p class="author"> Author: Bob Mottram
<a href="mailto:motters@bigblue">&lt;motters@bigblue&gt;</a>
</p>
<p class="date"> Date: 2013-01-17 17:17:37 GMT</p>
<p class="creator">HTML generated by org-mode 6.33x in emacs 23</p>
</div>
</div>
</body>
</html>
