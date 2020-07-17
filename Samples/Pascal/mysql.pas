unit mysql;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Grids, ADODb, Db, ComObj;

const
        META_TABLE_WORKER = 'worker';
type
  TForm1 = class(TForm)
    Label1: TLabel;
    Edit1: TEdit;
    Button1: TButton;
    GroupBox1: TGroupBox;
    StringGrid1: TStringGrid;
    Label2: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure HandleError(E: Exception);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.Button1Click(Sender: TObject);
var
        Connection1: TADOConnection;
        Table1: TADOTable;
        ConnStr: WideString;
        i: Integer;
begin
        if Edit1.Text = '' then
        begin
                Application.MessageBox('Не задано имя сервера !', 'Ошибка',
                                        MB_OK);
                Exit;
        end;
        ConnStr := 'Provider=MySQL.1;Server=' + Edit1.Text + ';Initial Catalog=dbtest;Password="";User ID=root;';

        // создание объектов
        Connection1 := TADOConnection.Create(nil);
        Table1 := TADOTable.Create(nil);
        // задание свойств
        with Connection1 do
        begin
                Provider := 'MySql.1';
                CursorLocation := clUseServer;
                ConnectionString := ConnStr;
                LoginPrompt := FALSE;
        end;
        with Table1 do
        begin
                Connection := Connection1;
                CursorType := ctOpenForwardOnly;
                LockType := ltReadOnly;
                TableName := META_TABLE_WORKER;
        end;

        try
                try
                Connection1.Connected := TRUE;
                Table1.Active := TRUE;
                // заполнение таблицы
                StringGrid1.Cells[0, 0] := 'Номер';
                StringGrid1.Cells[1, 0] := 'Имя';
                StringGrid1.Cells[2, 0] := 'Возраст';
                StringGrid1.RowCount := 2;
                while not Table1.Eof do
                begin
                        i := StringGrid1.RowCount;
                        // id
                        StringGrid1.Cells[0, i] := Table1.Fields[0].AsString;
                        // name
                        StringGrid1.Cells[1, i] := Table1.Fields[1].AsString;
                        // age
                        StringGrid1.Cells[2, i] := Table1.Fields[2].AsString;

                        StringGrid1.RowCount := i + 1;
                        Table1.Next;
                end;
                finally
                        Connection1.Free;
                        Table1.Free;
                end;
        except
                on EOleException do HandleError(Exception(ExceptObject));
        end;
end;

procedure TForm1.HandleError(E: Exception);
var
        Msg: array [0..1024] of char;
begin
        StrPCopy(Msg, E.Message);
        Application.MessageBox(Msg, 'Ошибка', MB_OK);
end;
end.
