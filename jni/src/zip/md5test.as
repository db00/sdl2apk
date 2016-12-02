/**

  ~/flex_sdk_4.5/bin/mxmlc -library-path+=md5.swc md5test.as -o md5.swf && ~/flex_sdk_4.5/bin/flashplayerdebugger md5.swf
/home/libiao/flex_sdk_4.5/bin/mxmlc -source-path=. -library-path+=md5.swc ./md5test.as -o test.swf 

 */
package {
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.net.URLLoaderDataFormat;

	import cmodule.md5.CLibInit;

	public class md5test extends Sprite
	{
		private var text:TextField = new TextField();
		public function md5test ()
		{
			var loaders:CLibInit = new CLibInit();
			var lib:Object = loaders.init();
			var s:String = "e10adc3949ba59abbe56e057f20f883e";
			var s2:String = lib.md5("123456");
			text.text = s2 + "\n" + s + "\n" + String(s==s2);
			trace(text.text);
			addChild(text);
			text.width = stage.stageWidth;
			text.height = stage.stageHeight;
			text.border = true;
			text.wordWrap = true;
		}
	}
}
