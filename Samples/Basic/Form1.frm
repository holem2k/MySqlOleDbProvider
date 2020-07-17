VERSION 5.00
Begin VB.Form Form1 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Пример доступа к серверу MySQL из языка Бейсик"
   ClientHeight    =   3615
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   7215
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3615
   ScaleWidth      =   7215
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.Frame Frame1 
      Caption         =   "Информация"
      Height          =   2055
      Left            =   120
      TabIndex        =   4
      Top             =   1440
      Width           =   6975
      Begin VB.TextBox Text3 
         Height          =   285
         Left            =   240
         TabIndex        =   8
         Text            =   "0"
         Top             =   1320
         Width           =   2655
      End
      Begin VB.TextBox Text2 
         Height          =   285
         Left            =   240
         TabIndex        =   7
         Top             =   600
         Width           =   2655
      End
      Begin VB.Label Label4 
         Caption         =   "Возраст"
         Height          =   255
         Left            =   240
         TabIndex        =   6
         Top             =   1080
         Width           =   1935
      End
      Begin VB.Label Label3 
         Caption         =   "Имя"
         Height          =   255
         Left            =   240
         TabIndex        =   5
         Top             =   360
         Width           =   1695
      End
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Добавить информацию"
      Height          =   255
      Left            =   3120
      TabIndex        =   2
      Top             =   960
      Width           =   3975
   End
   Begin VB.TextBox Text1 
      Height          =   285
      Left            =   120
      TabIndex        =   1
      Text            =   "127.0.0.1"
      Top             =   960
      Width           =   2655
   End
   Begin VB.Label Label2 
      Caption         =   "Добавление информации в таблицу"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   204
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   5775
   End
   Begin VB.Label Label1 
      Caption         =   "Сервер"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   600
      Width           =   855
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
    ' проверка имени сервера
    If Text1.Text = "" Then
        MsgBox "Неверное имя сервера", vbOKOnly, "Ошибка"
        Exit Sub
    End If
    
    ' проверка добавляемой информации
    If Text2.Text = "" Or Text3.Text = "" Then
        MsgBox "Необходимо ввести имя и возраст !"
        Exit Sub
    End If
    
    On Error GoTo Error_Db:
    
    Dim strCnxn As String
    strCnxn = "Provider=MySQL.1;Data Source=" + Text1.Text + ";Initial Catalog=dbtest;Password='';User ID=root;Extended Properties=''"

    ' создание объектов
    Dim Conn As New ADODB.Connection
    Dim Command As New ADODB.Command
    Dim Records As Integer
    
    ' задание свойств соединения
    Conn.CursorLocation = adUseServer
    Conn.ConnectionString = strCnxn
    ' открытие соединения
    Conn.Open
    
    Command.ActiveConnection = Conn
    Command.CommandType = adCmdText
    Command.CommandText = "INSERT INTO worker (name, age)" & _
                          "VALUES ('" + Text2.Text + "', " & _
                          Text3.Text + ")"
    Command.Execute Records, , adExecuteNoRecords
    'Command.Execute Records
    
    If Records = 1 Then
        MsgBox "Информация добавлена в БД !", vbOKOnly, "Сообщение"
    End If
    
    Exit Sub
    
    ' обработка ошибок
Error_Db:
    Dim Err As ADODB.Error
    If Conn.Errors.Count > 0 Then
        For Each Err In Conn.Errors
            MsgBox "Error number: " & Err.Number & vbCr & _
                Err.Description
        Next Err
    End If
End Sub
