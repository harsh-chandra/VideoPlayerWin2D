﻿#pragma checksum "C:\Users\graphics_lab\Desktop\Harsh\FFmpegInterop\Samples\SamplesWin10\MediaPlayerCS\MainPage.xaml" "{406ea660-64cf-4c82-b6f0-42d48172a799}" "DC322DA754C4DF25B5380180380F3CCA"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace MediaPlayerCS
{
    partial class MainPage : 
        global::Windows.UI.Xaml.Controls.Page, 
        global::Windows.UI.Xaml.Markup.IComponentConnector,
        global::Windows.UI.Xaml.Markup.IComponentConnector2
    {
        /// <summary>
        /// Connect()
        /// </summary>
        [global::System.CodeDom.Compiler.GeneratedCodeAttribute("Microsoft.Windows.UI.Xaml.Build.Tasks"," 14.0.0.0")]
        [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
        public void Connect(int connectionId, object target)
        {
            switch(connectionId)
            {
            case 1:
                {
                    this.Splitter = (global::Windows.UI.Xaml.Controls.SplitView)(target);
                }
                break;
            case 2:
                {
                    this.Hamburger = (global::Windows.UI.Xaml.Controls.FontIcon)(target);
                }
                break;
            case 3:
                {
                    this.Header = (global::Windows.UI.Xaml.Controls.TextBlock)(target);
                }
                break;
            case 4:
                {
                    this.toggleSwitchAudioDecode = (global::Windows.UI.Xaml.Controls.ToggleSwitch)(target);
                }
                break;
            case 5:
                {
                    this.toggleSwitchVideoDecode = (global::Windows.UI.Xaml.Controls.ToggleSwitch)(target);
                }
                break;
            case 6:
                {
                    global::Windows.UI.Xaml.Controls.AppBarButton element6 = (global::Windows.UI.Xaml.Controls.AppBarButton)(target);
                    #line 38 "..\..\..\MainPage.xaml"
                    ((global::Windows.UI.Xaml.Controls.AppBarButton)element6).Click += this.OpenLocalFile;
                    #line default
                }
                break;
            case 7:
                {
                    global::Windows.UI.Xaml.Controls.TextBox element7 = (global::Windows.UI.Xaml.Controls.TextBox)(target);
                    #line 39 "..\..\..\MainPage.xaml"
                    ((global::Windows.UI.Xaml.Controls.TextBox)element7).KeyUp += this.URIBoxKeyUp;
                    #line default
                }
                break;
            case 8:
                {
                    this.mediaElement = (global::Windows.UI.Xaml.Controls.MediaElement)(target);
                    #line 51 "..\..\..\MainPage.xaml"
                    ((global::Windows.UI.Xaml.Controls.MediaElement)this.mediaElement).MediaFailed += this.MediaFailed;
                    #line default
                }
                break;
            default:
                break;
            }
            this._contentLoaded = true;
        }

        [global::System.CodeDom.Compiler.GeneratedCodeAttribute("Microsoft.Windows.UI.Xaml.Build.Tasks"," 14.0.0.0")]
        [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
        public global::Windows.UI.Xaml.Markup.IComponentConnector GetBindingConnector(int connectionId, object target)
        {
            global::Windows.UI.Xaml.Markup.IComponentConnector returnValue = null;
            return returnValue;
        }
    }
}

