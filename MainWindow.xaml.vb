Class MainWindow

#Region "Publics"

    'variables to hold operands
    Private valHolder1 As Double
    Private valHolder2 As Double
    'Varible to hold temporary values
    Private tmpValue As Double
    'variable for Memory storage
    Private Memory As Double
    'True if "." is use else false
    Private hasDecimal As Boolean
    Private inputStatus As Boolean
    Private clearText As Boolean
    'variable to hold Operater
    Private calcFunc As String

#End Region

#Region "MainWindow"

    Private Sub MainWindow_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles Me.Closing
        Dim p As New Point(Me.Left, Me.Top)
        My.Settings.WindowLoc = p
        My.Settings.Save()
    End Sub

    Private Sub MainWindow_Initialized(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Initialized
        Me.Top = My.Settings.WindowLoc.Y
        Me.Left = My.Settings.WindowLoc.X
    End Sub

    Private Sub MainWindow_Loaded(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs) Handles MyBase.Loaded
        If My.Computer.Info.OSVersion >= "6.0" Then
            GlassHelper.ExtendGlassFrame(Me, New Thickness(-1, -1, -1, -1))
        End If
    End Sub

#End Region

#Region "Menu"

    Private Sub AboutMenuItem_Click(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs) Handles AboutMenuItem.Click
        Dim a As New AboutDialog
        a.Owner = Me
        a.ShowDialog()
    End Sub

#End Region

#Region "Number Buttons"

    Private Sub cmd9_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button9.Click
        If inputStatus = False Then
            TextInputBox.Text += "9"
        Else
            TextInputBox.Text = "9"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd8_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button8.Click
        If inputStatus = False Then
            TextInputBox.Text += "8"
        Else
            TextInputBox.Text = "8"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd7_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button7.Click
        If inputStatus = False Then
            TextInputBox.Text += "7"
        Else
            TextInputBox.Text = "7"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd6_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button6.Click
        If inputStatus = False Then
            TextInputBox.Text += "6"
        Else
            TextInputBox.Text = "6"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd5_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button5.Click
        If inputStatus = False Then
            TextInputBox.Text += "5"
        Else
            TextInputBox.Text = "5"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd4_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button4.Click
        If inputStatus = False Then
            TextInputBox.Text += "4"
        Else
            TextInputBox.Text = "4"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button3.Click
        If inputStatus = False Then
            TextInputBox.Text += "3"
        Else
            TextInputBox.Text = "3"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        If inputStatus = False Then
            TextInputBox.Text += "2"
        Else
            TextInputBox.Text = "2"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        If inputStatus = False Then
            TextInputBox.Text += "1"
        Else
            TextInputBox.Text = "1"
            inputStatus = False
        End If
    End Sub

    Private Sub cmd0_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button0.Click
        If inputStatus = False Then
            If TextInputBox.Text.Length >= 1 Then
                TextInputBox.Text += "0"
            End If
        End If
    End Sub

#End Region

#Region "Calculation Buttons"

    Private Sub cmdAdd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonAdd.Click
        If TextInputBox.Text.Length <> 0 Then
            If calcFunc = String.Empty Then
                valHolder1 = CDbl(TextInputBox.Text)
                TextInputBox.Text = String.Empty
            Else
                CalculateTotals()
            End If
            calcFunc = "Add"
            hasDecimal = False
        End If
    End Sub

    Private Sub cmdSubtract_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonSubtract.Click
        If TextInputBox.Text.Length <> 0 Then
            If calcFunc = String.Empty Then
                valHolder1 = CDbl(TextInputBox.Text)
                TextInputBox.Text = String.Empty
            Else
                CalculateTotals()
            End If
            calcFunc = "Subtract"
            hasDecimal = False
        End If
    End Sub

    Private Sub cmdDivide_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonDivide.Click
        If TextInputBox.Text.Length <> 0 Then
            If calcFunc = String.Empty Then
                valHolder1 = CDbl(TextInputBox.Text)
                TextInputBox.Text = String.Empty
            Else
                CalculateTotals()
            End If
            calcFunc = "Divide"
            hasDecimal = False
        End If
    End Sub

    Private Sub cmdMultiply_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonMultiply.Click
        If TextInputBox.Text.Length <> 0 Then
            If calcFunc = String.Empty Then
                valHolder1 = CDbl(TextInputBox.Text)
                TextInputBox.Text = String.Empty
            Else
                CalculateTotals()
            End If
            calcFunc = "Multiply"
            hasDecimal = False
        End If
    End Sub

    Private Sub cmdPowerOf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonPowerOf.Click
        If TextInputBox.Text.Length <> 0 Then
            If calcFunc = String.Empty Then
                valHolder1 = CDbl(TextInputBox.Text)
                TextInputBox.Text = String.Empty
            Else
                CalculateTotals()
            End If
            calcFunc = "PowerOf"
            hasDecimal = False
        End If
    End Sub

    Private Sub cmdSqrRoot_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonSqrRoot.Click
        If TextInputBox.Text.Length <> 0 Then
            tmpValue = CDbl(TextInputBox.Text)
            tmpValue = System.Math.Sqrt(tmpValue)
            TextInputBox.Text = CStr(tmpValue)
            hasDecimal = False
        End If
    End Sub

    Private Sub cmdEqual_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonEqual.Click
        If TextInputBox.Text.Length <> 0 AndAlso valHolder1 <> 0 Then
            CalculateTotals()
            calcFunc = ""
            hasDecimal = False
        End If
    End Sub

    Private Sub cmdInverse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonInverse.Click
        If TextInputBox.Text.Length <> 0 Then
            tmpValue = CDbl(TextInputBox.Text)
            tmpValue = 1 / tmpValue
            TextInputBox.Text = CStr(tmpValue)
            hasDecimal = False
        End If
    End Sub

    'Private Sub cmdSign_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdSign.Click
    '    If inputStatus = False Then
    '        If TextInputBox.Text.Length > 0 Then
    '            valHolder1 = -1 * CDbl(TextInputBox.Text)
    '            TextInputBox.Text = CStr(valHolder1)
    '        End If
    '    End If
    'End Sub

#End Region

#Region "Other Buttons"

    Private Sub cmdClearEntry_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CEButton.Click
        TextInputBox.Text = String.Empty
        hasDecimal = False
    End Sub

    Private Sub cmdClearAll_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CButton.Click
        TextInputBox.Text = String.Empty
        valHolder1 = 0
        valHolder2 = 0
        calcFunc = String.Empty
        hasDecimal = False
    End Sub

    Private Sub cmdBackspace_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles BackButton.Click
        Dim str As String
        Dim loc As Integer
        If TextInputBox.Text.Length > 0 Then
            str = TextInputBox.Text.Chars(TextInputBox.Text.Length - 1)
            If str = "." Then
                hasDecimal = False
            End If
            loc = TextInputBox.Text.Length
            TextInputBox.Text = TextInputBox.Text.Remove(loc - 1, 1)
        End If
    End Sub

    Private Sub cmdDecimal_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonDecimal.Click
        'Check for input status (we want flase)
        If Not inputStatus Then
            'Check if it already has a decimal (if it does then do nothing)
            If Not hasDecimal Then
                'Check to make sure the length is > than 1
                'Dont want user to add decimal as first character
                If TextInputBox.Text.Length > 1 Then
                    'Make sure 0 isnt the first number
                    If Not TextInputBox.Text = "0" Then
                        'It met all our requirements so add the zero
                        TextInputBox.Text += ButtonDecimal.Content
                        'Toggle the flag to true (only 1 decimal per calculation)
                        hasDecimal = True
                    End If
                Else
                    TextInputBox.Text = "0."
                End If
            End If
        End If
    End Sub

#End Region

#Region "Helpers"

    Private Sub CalculateTotals()
        valHolder2 = CDbl(TextInputBox.Text)
        Select Case calcFunc
            Case "Add"
                valHolder1 = valHolder1 + valHolder2
            Case "Subtract"
                valHolder1 = valHolder1 - valHolder2
            Case "Divide"
                valHolder1 = valHolder1 / valHolder2
            Case "Multiply"
                valHolder1 = valHolder1 * valHolder2
            Case "PowerOf"
                valHolder1 = System.Math.Pow(valHolder1, valHolder2)
        End Select
        TextInputBox.Text = CStr(valHolder1)
        inputStatus = True
    End Sub

#End Region

End Class