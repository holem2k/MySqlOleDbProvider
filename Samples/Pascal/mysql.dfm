object Form1: TForm1
  Left = 64
  Top = 107
  BorderStyle = bsDialog
  Caption = 'Пример доступа к серверу MySQL из языка Паскаль'
  ClientHeight = 308
  ClientWidth = 514
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 35
    Width = 37
    Height = 13
    Caption = 'Сервер'
  end
  object Label2: TLabel
    Left = 8
    Top = 6
    Width = 138
    Height = 16
    Caption = 'Запрос данных из БД'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Edit1: TEdit
    Left = 8
    Top = 51
    Width = 225
    Height = 21
    TabOrder = 0
    Text = '127.0.0.1'
  end
  object Button1: TButton
    Left = 245
    Top = 50
    Width = 260
    Height = 25
    Caption = 'Исполнить запрос'
    TabOrder = 1
    OnClick = Button1Click
  end
  object GroupBox1: TGroupBox
    Left = 2
    Top = 86
    Width = 503
    Height = 211
    Caption = 'Результаты запроса'
    TabOrder = 2
    object StringGrid1: TStringGrid
      Left = 8
      Top = 16
      Width = 491
      Height = 185
      ColCount = 3
      FixedCols = 0
      RowCount = 2
      TabOrder = 0
      ColWidths = (
        95
        175
        214)
    end
  end
end
