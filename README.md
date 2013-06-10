<html>
<body>
<div id="content">
<h1 class="title">stylom - A command line utility for stylometric text analysis</h1>
<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <span class="underline">Description</span></a></li>
<li><a href="#sec-2">2. <span class="underline">Installation</span></a></li>
<li><a href="#sec-3">3. <span class="underline">Generating Author Fingerprints</span></a></li>
<li><a href="#sec-4">4. <span class="underline">Matching Unknown Text</span></a></li>
<li><a href="#sec-5">5. <span class="underline">Counter-Stylometry</span></a></li>
<li><a href="#sec-6">6. <span class="underline">Plotting Text Distributions</span></a></li>
<li><a href="#sec-7">7. <span class="underline">Options</span></a>
<ul>
<li><a href="#sec-7-1">7.1. -t,&#x2013;text &lt;string or filename&gt;</a></li>
<li><a href="#sec-7-2">7.2. -n,&#x2013;name &lt;author&gt;</a></li>
<li><a href="#sec-7-3">7.3. -f,&#x2013;fingerprint</a></li>
<li><a href="#sec-7-4">7.4. -m,&#x2013;match &lt;directory&gt;</a></li>
<li><a href="#sec-7-5">7.5. -l,&#x2013;list &lt;directory&gt;</a></li>
<li><a href="#sec-7-6">7.6. -c,&#x2013;compare &lt;fingerprint&gt;</a></li>
<li><a href="#sec-7-7">7.7. &#x2013;missing &lt;fingerprint&gt;</a></li>
<li><a href="#sec-7-8">7.8. &#x2013;more &lt;fingerprint&gt;</a></li>
<li><a href="#sec-7-9">7.9. &#x2013;less &lt;fingerprint&gt;</a></li>
<li><a href="#sec-7-10">7.10. -p,&#x2013;plot &lt;directory&gt;</a></li>
<li><a href="#sec-7-11">7.11. &#x2013;plottexts &lt;directory&gt;</a></li>
<li><a href="#sec-7-12">7.12. &#x2013;title &lt;text&gt;</a></li>
<li><a href="#sec-7-13">7.13. -v,&#x2013;version</a></li>
<li><a href="#sec-7-14">7.14. -h,&#x2013;help</a></li>
</ul>
</li>
<li><a href="#sec-8">8. <span class="underline">Bugs</span></a></li>
<li><a href="#sec-9">9. <span class="underline">Author</span></a></li>
</ul>
</div>
</div>

<div id="outline-container-sec-1" class="outline-2">
<h2 id="sec-1"><span class="section-number-2">1</span> <span class="underline">Description</span></h2>
<div class="outline-text-2" id="text-1">
<p>
This is a tool to facilitate the stylometric analysis of texts.  It could be used for academic disambiguation of disputed authorship, and to help identify plagiarists, astroturfers, sockpuppets and guerilla marketers.  Another possible use case is as an assistance to the anonymisation of writing style.
</p>

<p>
Every author has their own unique writing style, and if enough writing examples are available then it's possible to construct a quantitative model of their style which can be compared against others.
</p>
</div>
</div>

<div id="outline-container-sec-2" class="outline-2">
<h2 id="sec-2"><span class="section-number-2">2</span> <span class="underline">Installation</span></h2>
<div class="outline-text-2" id="text-2">
<p>
The easiest way to install is from a pre-compiled package, a number of which are available at:
</p>

<div class="org-src-container">

<pre class="src src-sh">https://build.opensuse.org/project/show?project=home%3Amotters%3Astylom
</pre>
</div>

<p>
But if you prefer not to get involved with binaries then this program is pretty easy to compile and install as follows:
</p>

<div class="org-src-container">

<pre class="src src-sh">make
sudo make install
</pre>
</div>

<p>
If you wish to generate a Debian package you can also run the following script:
</p>

<div class="org-src-container">

<pre class="src src-sh">./debian.sh
</pre>
</div>

<p>
To plot graphs you will need to have gnuplot installed. For example:
</p>

<div class="org-src-container">

<pre class="src src-sh">sudo apt-get install gnuplot
</pre>
</div>
</div>
</div>

<div id="outline-container-sec-3" class="outline-2">
<h2 id="sec-3"><span class="section-number-2">3</span> <span class="underline">Generating Author Fingerprints</span></h2>
<div class="outline-text-2" id="text-3">
<p>
Fingerprints are high dimensional vectors which represent the writing style of particular authors.  To generate a fingerprint first gather some examples of the author's writing within plain text files and put them in a directory.  Then run something similar to the following:
</p>

<div class="org-src-container">

<pre class="src src-sh">cat texts/dickens/* | stylom -n "Charles Dickens" -f &gt; fingerprints/dickens.style
</pre>
</div>

<p>
Here the texts get piped into the command and the resulting fingerprint is then saved to a file.  Ideally the amount of example text should be as large as possible.
</p>
</div>
</div>

<div id="outline-container-sec-4" class="outline-2">
<h2 id="sec-4"><span class="section-number-2">4</span> <span class="underline">Matching Unknown Text</span></h2>
<div class="outline-text-2" id="text-4">
<p>
If you have a file containing a sample of text for which the author is unknown, but who is likely to be an author for which you have previously generated a fingerprint, you can find the most likely candidate in the following way:
</p>

<div class="org-src-container">

<pre class="src src-sh">cat unknown.txt | stylom --match fingerprints
</pre>
</div>

<p>
Where <i>fingerprints</i> is the name of a directory containing previously saved fingerprints for a variety of possible authors.  This will return a single name, but it's also possible to return a list of candidates in the following way.
</p>

<div class="org-src-container">

<pre class="src src-sh">cat unknown.txt | stylom --list fingerprints
</pre>
</div>
</div>
</div>

<div id="outline-container-sec-5" class="outline-2">
<h2 id="sec-5"><span class="section-number-2">5</span> <span class="underline">Counter-Stylometry</span></h2>
<div class="outline-text-2" id="text-5">
<p>
Stylometrics have various legitimate uses in terms of the study of disputed historical texts or detecting plagiarism.  However, a possible danger of stylometric methods is their abuse by powerful organisations against bloggers or whistleblowers who may be trying to raise issues of public concern in an anonymous manner in order to avoid serious reprisals.
</p>

<p>
A known method to render stylometry less effective in determining the identity of an author is to try to make the style of writing as similar as possible to some existing author, hence obscuring the differences.  Automatically transforming the writing style of a given text into the writing style of a known author is a very difficult and likely AI-complete problem, but this program can be used provide the writer with advice on how to alter their work to achieve a greater degree of similarity.
</p>

<div class="org-src-container">

<pre class="src src-sh">cat mytext.txt | stylom -c fingerprints/charles_dickens.style
</pre>
</div>

<p>
or
</p>

<div class="org-src-container">

<pre class="src src-sh">stylom -t mytext.txt -c fingerprints/charles_dickens.style
</pre>
</div>

<p>
Takes some text and compares it to a pre-computed fingerprint for Charles Dickens.  It then gives some indication of how to alter <i>mytext.txt</i> in order to make it more similar to Dickens' writing style.  The number of differences is also shown, which can be used as an indicator of progress.
</p>

<p>
If you are only interested in which words are present in <i>mytext.txt</i> but missing from Dickens writing:
</p>

<div class="org-src-container">

<pre class="src src-sh">cat mytext.txt | stylom --missing fingerprints/charles_dickens.style
</pre>
</div>

<p>
or
</p>

<div class="org-src-container">

<pre class="src src-sh">stylom -t mytext.txt --missing fingerprints/charles_dickens.style
</pre>
</div>

<p>
The results could then be used by some other program (maybe highlighted in a text editor GUI).  The same can also be done for words which are more frequent within the fingerprint or less frequent within the fingerprint.
</p>

<div class="org-src-container">

<pre class="src src-sh">cat mytext.txt | stylom --more fingerprints/charles_dickens.style
</pre>
</div>

<p>
or
</p>

<div class="org-src-container">

<pre class="src src-sh">cat mytext.txt | stylom --less fingerprints/charles_dickens.style
</pre>
</div>

<p>
The above methods can be used to make the vocabulary and the word frequency similar to an existing known author.  It's not a perfect solution, since it takes no account of syntax, and it still involves mannual editing effort by the writer.
</p>
</div>
</div>

<div id="outline-container-sec-6" class="outline-2">
<h2 id="sec-6"><span class="section-number-2">6</span> <span class="underline">Plotting Text Distributions</span></h2>
<div class="outline-text-2" id="text-6">
<p>
You can plot the distribution of fingerprints in the following way.
</p>

<div class="org-src-container">

<pre class="src src-sh">stylom --plot &lt;directory&gt;
</pre>
</div>

<p>
This plots any fingerprints within the given directory within a 2D graph so that you can see how authors are distributed.  The graph is saved by default with the filename <i>result.png</i>
</p>

<p>
If necessary you can also specify a title for the graph.
</p>

<div class="org-src-container">

<pre class="src src-sh">stylom --title "My Graph Title" --plot &lt;directory&gt;
</pre>
</div>

<p>
You can also plot texts more directly without having previously calculated fingerprints for them.
</p>

<div class="org-src-container">

<pre class="src src-sh">stylom --title "My Texts" --plottexts &lt;directory&gt;
</pre>
</div>
</div>
</div>

<div id="outline-container-sec-7" class="outline-2">
<h2 id="sec-7"><span class="section-number-2">7</span> <span class="underline">Options</span></h2>
<div class="outline-text-2" id="text-7">
</div><div id="outline-container-sec-7-1" class="outline-3">
<h3 id="sec-7-1"><span class="section-number-3">7.1</span> -t,&#x2013;text &lt;string or filename&gt;</h3>
<div class="outline-text-3" id="text-7-1">
<p>
This can be either text to be analysed or a filename containing plain text.
</p>
</div>
</div>
<div id="outline-container-sec-7-2" class="outline-3">
<h3 id="sec-7-2"><span class="section-number-3">7.2</span> -n,&#x2013;name &lt;author&gt;</h3>
<div class="outline-text-3" id="text-7-2">
<p>
The name of the author.
</p>
</div>
</div>
<div id="outline-container-sec-7-3" class="outline-3">
<h3 id="sec-7-3"><span class="section-number-3">7.3</span> -f,&#x2013;fingerprint</h3>
<div class="outline-text-3" id="text-7-3">
<p>
Displays a fingerprint on the console in XML format.
</p>
</div>
</div>
<div id="outline-container-sec-7-4" class="outline-3">
<h3 id="sec-7-4"><span class="section-number-3">7.4</span> -m,&#x2013;match &lt;directory&gt;</h3>
<div class="outline-text-3" id="text-7-4">
<p>
Match the given fingerprint against ones stored within a directory.
</p>
</div>
</div>
<div id="outline-container-sec-7-5" class="outline-3">
<h3 id="sec-7-5"><span class="section-number-3">7.5</span> -l,&#x2013;list &lt;directory&gt;</h3>
<div class="outline-text-3" id="text-7-5">
<p>
Match the given fingerprint against ones stored within a directory and show a list of the most similar authors.
</p>
</div>
</div>
<div id="outline-container-sec-7-6" class="outline-3">
<h3 id="sec-7-6"><span class="section-number-3">7.6</span> -c,&#x2013;compare &lt;fingerprint&gt;</h3>
<div class="outline-text-3" id="text-7-6">
<p>
Compares the current text against the given fingerprint and reports differences.
</p>
</div>
</div>
<div id="outline-container-sec-7-7" class="outline-3">
<h3 id="sec-7-7"><span class="section-number-3">7.7</span> &#x2013;missing &lt;fingerprint&gt;</h3>
<div class="outline-text-3" id="text-7-7">
<p>
Report words which are present in the current text but which are missing from the given fingerprint.
</p>
</div>
</div>
<div id="outline-container-sec-7-8" class="outline-3">
<h3 id="sec-7-8"><span class="section-number-3">7.8</span> &#x2013;more &lt;fingerprint&gt;</h3>
<div class="outline-text-3" id="text-7-8">
<p>
Report words which are present more frequently in the given fingerprint than in the current text.
</p>
</div>
</div>
<div id="outline-container-sec-7-9" class="outline-3">
<h3 id="sec-7-9"><span class="section-number-3">7.9</span> &#x2013;less &lt;fingerprint&gt;</h3>
<div class="outline-text-3" id="text-7-9">
<p>
Report words which are present less frequently in the given fingerprint than in the current text.
</p>
</div>
</div>
<div id="outline-container-sec-7-10" class="outline-3">
<h3 id="sec-7-10"><span class="section-number-3">7.10</span> -p,&#x2013;plot &lt;directory&gt;</h3>
<div class="outline-text-3" id="text-7-10">
<p>
Plots fingerprints within the given directory using gnuplot.  The resulting image is saved as <i>result.png</i>
</p>
</div>
</div>
<div id="outline-container-sec-7-11" class="outline-3">
<h3 id="sec-7-11"><span class="section-number-3">7.11</span> &#x2013;plottexts &lt;directory&gt;</h3>
<div class="outline-text-3" id="text-7-11">
<p>
Plots texts within the given directory using gnuplot.  The resulting image is saved as <i>result.png</i>
</p>
</div>
</div>
<div id="outline-container-sec-7-12" class="outline-3">
<h3 id="sec-7-12"><span class="section-number-3">7.12</span> &#x2013;title &lt;text&gt;</h3>
<div class="outline-text-3" id="text-7-12">
<p>
Specify a title for use together with the &#x2013;plot option.
</p>
</div>
</div>
<div id="outline-container-sec-7-13" class="outline-3">
<h3 id="sec-7-13"><span class="section-number-3">7.13</span> -v,&#x2013;version</h3>
<div class="outline-text-3" id="text-7-13">
<p>
Show the version number.
</p>
</div>
</div>
<div id="outline-container-sec-7-14" class="outline-3">
<h3 id="sec-7-14"><span class="section-number-3">7.14</span> -h,&#x2013;help</h3>
<div class="outline-text-3" id="text-7-14">
<p>
Show help.
</p>
</div>
</div>
</div>

<div id="outline-container-sec-8" class="outline-2">
<h2 id="sec-8"><span class="section-number-2">8</span> <span class="underline">Bugs</span></h2>
<div class="outline-text-2" id="text-8">
<p>
Report all bugs to <i><a href="https://github.com/fuzzgun/stylom/issues">https://github.com/fuzzgun/stylom/issues</a></i>
</p>
</div>
</div>

<div id="outline-container-sec-9" class="outline-2">
<h2 id="sec-9"><span class="section-number-2">9</span> <span class="underline">Author</span></h2>
<div class="outline-text-2" id="text-9">
<p>
Bob Mottram &lt;bob@robotics.uk.to&gt;
</p>

<p>
GPG ID: 0xEA982E38
</p>

<p>
GPG Fingerprint: D538 1159 CD7A 2F80 2F06  ABA0 0452 CC7C EA98 2E38
</p>

<p>
<a href="http://robotics.uk.to">http://robotics.uk.to</a>
</p>
</div>
</div>
</div>
<div id="postamble" class="status">
<p class="author">Author: Bob Mottram</p>
<p class="date">Created: 2013-06-10 Mon 15:48</p>
<p class="creator"><a href="http://www.gnu.org/software/emacs/">Emacs</a> 24.2.1 (<a href="http://orgmode.org">Org</a> mode 8.0.3)</p>
<p class="xhtml-validation"><a href="http://validator.w3.org/check?uri=referer">Validate XHTML 1.0</a></p>
</div>
</body>
</html>
