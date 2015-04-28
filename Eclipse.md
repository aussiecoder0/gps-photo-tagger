### Requirements ###

> If you want develop GPS Photo Tagger in Eclipse you need CDT and Subclipse plugins installed. CDT provides C++ development tools. Subclipse provides Subversion support. Homepage addresses of both plugins are above.

  * http://www.eclipse.org/cdt/
  * http://subclipse.tigris.org/

### Importing ###

> If you have both plugins installed start Eclipse and open your workspace. Than click _**File**_ / _**Import**_, select _**SVN**_ / _**Checkout Project from SVN**_ and click _**Next**_. In next window select _**Create a new repository location**_ and click _**Next**_. Fill _**Url**_ field with `https://gps-photo-tagger.googlecode.com/svn/trunk/` and click _**Next**_. Wait a while and select _**GpsLogger**_ directory and click _**Finish**_. Now you should see project _**GpsLogger**_ in your workspace.

### Building ###

> There are two build configurations prepared to work if all packages mentioned at [Compilation](Compilation.md) page are installed. These profiles are default _**Debug**_ and _**Release**_ configuration.

### Running outside IDE ###

> Please be sure you run application inside project root directory. It loads configuration files during start. If you run it inside _**Debug**_ or _**Release**_ directory it won't start.