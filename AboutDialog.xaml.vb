Partial Public Class AboutDialog

    Private Sub AboutDialog_Loaded(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs) Handles MyBase.Loaded
        If GlassHelper.DwmIsCompositionEnabled Then
            GlassHelper.ExtendGlassFrame(Me, New Thickness(-1, -1, -1, -1))
        End If
        NameLabel.Content = My.Application.Info.ProductName
        VersionLabel.Content = "Version: " + My.Application.Info.Version.Major.ToString + "." + My.Application.Info.Version.Minor.ToString
        CompanyLabel.Content = My.Application.Info.Copyright
        Label1.Content = My.Application.Info.CompanyName
    End Sub

    Private Sub OKButton_Click(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs) Handles OKButton.Click
        Close()
    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs) Handles Button1.Click
        Process.Start("http://semagsoft.com")
    End Sub
End Class
