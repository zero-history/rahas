/**********************************************************************
    CIP_SIS (Confidential Integer Processing)
    Copyright (C) 2022  Jayamine Alupotha

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ZKSC_PARAMS32_H
#define ZKSC_PARAMS32_H

#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

typedef int512_t zint_t;
typedef uint512_t zuint_t;
typedef int1024_t zint2_t;


#define L       32

#define N       256
#define n       5
#define mk      1
#define mv      1
#define mr      1
#define m       (mk + mv + mr)
#define D       (mv + mr)

#define N       256
#define K1      283
#define K11     282
#define K2      91
#define KMONT   285

#define GAMMA2_BITS  130
#define GAMMA1_BITS  128
#define ALPHA_BITS   12
#define TAU_BITS     7
#define TAU1_BITS    7
#define TAU2_BITS    28
#define TAU3_BITS    20

#define u_BYTES       ((K1 * N)/8)  // 1600
#define z_BYTES       (L*((ALPHA_BITS + 1) * N)/8)
#define R_BYTES       ((m - D) * ((TAU2_BITS + 1) * N)/8)
#define sig_BYTES     ((m - D) * ((TAU3_BITS + 1 + 1) * N)/8) // for two 17 bits

#define r_BYTES       ((N)/8 * (TAU_BITS + 1))
#define r1_BYTES      ((N)/8 * (TAU1_BITS + 1))
#define r2_BYTES      ((N)/8 * (TAU2_BITS + 1))
#define r3_BYTES      ((N)/8 * (TAU3_BITS + 1))
#define a_BYTES       ((N)/8 * (ALPHA_BITS + 1))

#define BETA           60  // number of +1 or -1 in the challenge polynomial
#define SEED_BYTES     32
#define CRH_BYTES      64

#define CHI            64

#define u_ERROR        (128 - 32)
#define u_chal_ERROR   128
#define t1_ERROR       (128 - 10)
#define t2_ERROR       (128 - 10)
#define t3_ERROR       128
#define pk_ERROR       u_ERROR
#define y_ERROR        128
#define aggr_ERROR     128

#define u_HIGHBITS        ((((K11 - u_ERROR + 1) * N)/8) * n)
#define u_chal_HIGHBITS   ((((K11 - u_chal_ERROR + 1) * N)/8) * n)
#define t1_HIGHBITS       ((((K11 - t1_ERROR + 1) * N)/8) * n)
#define t2_HIGHBITS       ((((K11 - t2_ERROR + 1) * N)/8) * n)
#define pk_HIGHBITS       ((((K11 - pk_ERROR + 1) * N)/8) * n)
#define y_HIGHBITS       ((((K11 - y_ERROR + 1) * N)/8) * n)
#define aggr_HIGHBITS    ((((K11 - aggr_ERROR + 1) * N)/8) * n)

#define HINTBITS       2
#define HINTBYTES      ((((HINTBITS) * N)/8) * n)

#define EXTRA_BITS  8


static const int512_t Q("15541351137805832567355695254588151253139254712417116170012023397832663521091869736961");
static const int512_t Q2("7770675568902916283677847627294075626569627356208558085006011698916331760545934868480");
// Zp = Integers(Q), min(Zp.zeta(N*2, all=True))
static const int512_t ROOT("110047119455493901099475395281769776735154522876170144943517083202581617091636349264");
// R = 2**244
static const int512_t R("62165404551223330269422781018352605012557018849668464680057997111644937126566671941632");
// MONT = R in [0, Q]
static const int512_t MONT("9903520314283042199192993788");
// inverse_mod(Q, R)
static const int512_t QINV("46639230514138011210433644059917642483039371496114876738164354734997833751691702304769");
// R - 1
static const int512_t FILTER("62165404551223330269422781018352605012557018849668464680057997111644937126566671941631");
// MONT^2/256 (MONT*MONT*inverse_mod(256, Q))% Q
static const int512_t F1("14570016691692968031895964301176774923703267765105635996142750027754205390735448997889");
// MONT/256  (MONT*inverse_mod(256, Q))% Q
static const int512_t F2("242833611528216133864932738352939863330300854881517440156476551217363035650651062272");


static const zint_t GAMMA2("1361129467683753853853498429727072845824"); // 2**132
static const zint_t GAMMA1("340282366920938463463374607431768211456"); // 2**128
static const zint_t ALPHA("4096"); // 2**11
static const zint_t TAU("128");
static const zint_t TAU1("128");
static const zint_t TAU2("268435456");
static const zint_t TAU3("1048576");

// Safe range for different operations
static const zint_t SR_RP_z(ALPHA - 1);
static const zint_t SR_RP_s(TAU2 - (BETA * TAU) - (BETA * BETA * TAU1));
//static const zint_t SR_MUL_z(ALPHA1 - MAX);
static const zint_t SR_MUL_s(TAU2 - (3 * BETA * BETA * TAU) - BETA * TAU1);
//static const zint_t SR_DIV_z(ALPHA1 - MAX);
static const zint_t SR_DIV_s(TAU2 - (4 * BETA * BETA * TAU) - BETA * TAU1);
static const zint_t SR_EQ_s(TAU3 - (2 * BETA * TAU));

// zeta_0 = (2^23 * root^(128)) mod q => 128 = bit_rev(0) index(1)
// zeta_1 = (2^23 * root^(129)) mod q => 129 = bit_rev(1) index()
// zeta_2 = (2^23 * root^(65)) mod q => 65 = bit_rev(2)
// 17592186044415 (index(0))
static const zint_t zetas[N] = {(int512_t) ("9903520314283042199192993788"),
                                (int512_t) ("-1719911966866084491784219739118918803573245034374629791213295220751526602635363028274"),
                                (int512_t) ("3321777166111404056202428231880497908017112644773544648435973407056233474510948031612"),
                                (int512_t) ("-955646641458413333671553566172609629432093688126892855790197958632173215298425608519"),
                                (int512_t) ("-2236396156922711385415279026619787390982003931274793068290311682326883139379975532369"),
                                (int512_t) ("-1272938719595338121766987588544541058190813065243653288091611734100839914589869640404"),
                                (int512_t) ("-4722477006412379901493679579037600828012078147737750645033548817052495193060968017698"),
                                (int512_t) ("-2193520053533416174746880660527443173195517507160663386630523201304471173666547564379"),
                                (int512_t) ("5849602866330778798628655516924439798932565538930937324414485207845011018103232116463"),
                                (int512_t) ("-5094058118635529061454058730129284245309168069584019950817616505450507721704731457443"),
                                (int512_t) ("-5229818033262826072248525658074222173195547174180769006274113588251053754189971493214"),
                                (int512_t) ("-5781741159792143158417263049096644225121483951084599296457478237197684195482002543707"),
                                (int512_t) ("-612855285184792357897552379773666075937585835665840117478357969615663763285718211939"),
                                (int512_t) ("5579902589473711230982842523230202102928689746744783228651287771315205487131847488235"),
                                (int512_t) ("7282823780601636451027978694779150478016088052109749527635170862383404010218830244331"),
                                (int512_t) ("-5711419842287054544716940742194643271647694598144964680644137997588321420131135813418"),
                                (int512_t) ("-6727097333631965556929783069823332498489378090527695770241386125922799833471197932243"),
                                (int512_t) ("248932117760465329206220217190922279133079092976868336441572193625579491131768580308"),
                                (int512_t) ("-4683749068261061419349697021262319830277752032477564714538587121914236675458685331012"),
                                (int512_t) ("-755230854944487036338939688646634449964176053867852499696526999413558522239815320469"),
                                (int512_t) ("-2099355364147777540907780629119735024895952763503032897598112717903502262451741350750"),
                                (int512_t) ("-2192210319080067117791284102554122928457552467678288447988324191032774631176811989581"),
                                (int512_t) ("3156587714094175144954398910052922362992955982867619320733891591613856770740298638091"),
                                (int512_t) ("-3864894275184020016224113772394285928339809775811223018257044339003670114006999935933"),
                                (int512_t) ("7037704402867862890015307449054573347000202530161452909704607118205480658422427296835"),
                                (int512_t) ("7325598349100428070643815029075001330087757912128225233301536845232702561646499106322"),
                                (int512_t) ("-6448463603176603883646501889719466939054072047100364156508014530090902280905043348393"),
                                (int512_t) ("-2238731409275524261069502508543984533454734913762557078450262475641122359418925072523"),
                                (int512_t) ("143579360408504611773852077744788039402716620646160188498994487460371186967469276781"),
                                (int512_t) ("-6486849189961914213165429713530281728441093998218746151630476578023811119169162352335"),
                                (int512_t) ("-1865464649841226843464046465274456562309173544044372821701932214005152562127220541614"),
                                (int512_t) ("4837213990166180351912510967945160336899141691187762188234180360681493618655997467682"),
                                (int512_t) ("-7464147108316570884973117249140748465118650287179343036597906246976178586426039838650"),
                                (int512_t) ("5997728570329653111589286318534602528777087473623254714410735901465393253933732937099"),
                                (int512_t) ("-7512967943296815886686636303486030903157972841894740185570781931164551183566338738664"),
                                (int512_t) ("-1477358329538823716685488059664811144588514852713892888570853691552300008709965763260"),
                                (int512_t) ("549352843403973194584175109685970345718787978383911109432596131997572826272353954858"),
                                (int512_t) ("4371783768884422559521574858251762213142268172975102257986094868014770357138523896567"),
                                (int512_t) ("-560448023220153184399701679430254809477276403741789884801188552971574016684563101678"),
                                (int512_t) ("-5088740733392430643831985907923654056981742520495357955340136967470641545690234879922"),
                                (int512_t) ("7359389576364265879220762402118337936448490586785163973042900917973751504332841832536"),
                                (int512_t) ("-1459883813914698305912116741032222883195160494059118270810891272293680992810639484052"),
                                (int512_t) ("-6306543206168025587473714037786850867429417330562333677333928565065157223882946092880"),
                                (int512_t) ("3398588986872809899656504743220511856842380629879355387759170877570081757551129669849"),
                                (int512_t) ("-5353936778205278018890488554680053022341952604061199819687438204548296885362874055582"),
                                (int512_t) ("5531615042385194677435019523710020959212152688154940784031192301498891134004110501950"),
                                (int512_t) ("-2226013623149049393426836128316095165341211902916983566628974521989483449874883489907"),
                                (int512_t) ("-3077715057212881758121217650713835532924878906738614480615562979928498375128097412073"),
                                (int512_t) ("-7383670448378521117879735484218703140162626737290894423384699748911550138644341730274"),
                                (int512_t) ("-7722497425875781651364654482117538498223717672212098573245124245055462797446557648695"),
                                (int512_t) ("4770160357601697034531384494652957341329844090214016643099609239166517282125398360769"),
                                (int512_t) ("1823763016758371319685774234862565962775253897392234749926804984654539593681473740824"),
                                (int512_t) ("4719353772961104637206873265690781712393428851105181587892335769401856999439905520774"),
                                (int512_t) ("2939386562749266650198028258344566478838141130665453078968066462607220907893366388797"),
                                (int512_t) ("2264385852666264625928644765978428961030597877213264122154839194585279263350819126137"),
                                (int512_t) ("1931090907532715076966864028815782861517391023693500553120039752504582933377674873514"),
                                (int512_t) ("5722217912738388130561556085873148941984099834486036427448121099325199988970793025292"),
                                (int512_t) ("2196543527573188383463690876153315136699199008421689933635838844737508334078379287992"),
                                (int512_t) ("600603149195432587754196706119772877264250144323947910234404001878519708639688342781"),
                                (int512_t) ("-1821276158823539924995022291531522617128843221818967530681991720855404818640675918397"),
                                (int512_t) ("5463251755965275334709460555017560347774598152447838919939699544606318369850393506048"),
                                (int512_t) ("-5298168764172247666638102642743879230215898152813049137804189552818227272510201412856"),
                                (int512_t) ("-5189624295120631829579131061270687076119974178416904706193376578794485117366730719950"),
                                (int512_t) ("2548450319037225449587336280553940804671071569365341098928846651067556744629758704523"),
                                (int512_t) ("2602030615018043683224255299383300890403232583317642616234733904977408241751832447796"),
                                (int512_t) ("3809309112190323724913385258789229587767316410086460776892965412958935635062590582164"),
                                (int512_t) ("-7769821864988376410703476547407591323858613105911102331480162070713372195169392620195"),
                                (int512_t) ("-2123346850434982482733170678708521718132448325470386195058994728244372089656991293881"),
                                (int512_t) ("-3413245502380501151439772186946660909892826949859200682909696364522344738440630409542"),
                                (int512_t) ("-7170594386295663595291854357886786059057759887819865706249348548491660758438107545612"),
                                (int512_t) ("-4176893145431933499511266766175970594521508132842088258455211803831811066362941931741"),
                                (int512_t) ("5401928529502308193406024278196118023783581395052611901628134848898853079487682638935"),
                                (int512_t) ("2784937808867419228860430505703368119322904317238528789706731209875391574247356399481"),
                                (int512_t) ("-5155346456516390880415909221764014911793172402921546942223090586759504676299660596414"),
                                (int512_t) ("7173899303470330883524856201376768070449341388323403375007518989503603283835176702527"),
                                (int512_t) ("-4310600292052043159555876636035290941001895278088743226066048970313427492647461436891"),
                                (int512_t) ("5417767462684081303998599267729086052610254034275432778596753290391883147455266376834"),
                                (int512_t) ("-3287024649839644337954186719453876890660421673623160645591980452852036078588874402385"),
                                (int512_t) ("-4929777453035628479447495913625511327094664909271346453604342110656125575518423168514"),
                                (int512_t) ("6441424840318185950906419249674283424707948257752173611491781656222701655065327659850"),
                                (int512_t) ("4484672476778151961126335648233016125454152826153380377796967219677089956506887230746"),
                                (int512_t) ("-5555521488023270433781280467894422909297075787517757292912962638828311872326828946664"),
                                (int512_t) ("-1547730649382594706886398169635116374825169391494243778241511224164708813677431164748"),
                                (int512_t) ("7742871289116677099683605963306790232578571470691655335137333365740797925775154868747"),
                                (int512_t) ("-4344797946513684603484650014394814159578330177305824803941051619636292644467351564554"),
                                (int512_t) ("1376422271904102999374677993064051240026636070491016660877731298443886508140640564081"),
                                (int512_t) ("5155836369493809613474987133474345896350057942721431308907223879276385358534414557067"),
                                (int512_t) ("5624494249204760115980319117293572656894159531021548848308536305538815708761119044692"),
                                (int512_t) ("-2696308636206174580351678524683606845926109325826351596765572992556605725338435251661"),
                                (int512_t) ("1857565080502012453893196201876609403086582727318167020621332171917406570317865613514"),
                                (int512_t) ("395103646839968910324337278167530082567918218039540433180311909574580864813874318886"),
                                (int512_t) ("-2005031020966933475747339415670911363281073142727869739043550074744114989394705742257"),
                                (int512_t) ("1238593433405430989077573689666905209555206353924436788756005528051343821210862933173"),
                                (int512_t) ("-6289657510747750681401572925971687017930310125007348559877930104719363906447746886758"),
                                (int512_t) ("7217825076471351113804438883526003698180443762500907498406267387937421958848625583844"),
                                (int512_t) ("-2544493078257483666419073691776445137217134303936771529267281930388603862533792210612"),
                                (int512_t) ("-4315406328780886492256744611744811588905752250773074110811506554774567593188725157750"),
                                (int512_t) ("1781928968496090837578668117045473270931973080901937445147483790364428614802890743644"),
                                (int512_t) ("1423423688881354316394793333431935594474105596440013404200747175551152668251422236619"),
                                (int512_t) ("440264209542553883360843279209748222622988491594948291943329702900287094771807694189"),
                                (int512_t) ("4436325047148433961001500878577831792492053249368135015386866412826729180038135008752"),
                                (int512_t) ("5054876570568464863626167410785802553620487596294301878968660314304065037865456487508"),
                                (int512_t) ("-3588741055105847050186590175960231810057772008977659788711135910686538082520388386300"),
                                (int512_t) ("-2291520432539248313350084804742418436749086841868093166694856150404133725346451170760"),
                                (int512_t) ("-3526397652481916906358726647851083345108691429190358532463919400952697173572357538683"),
                                (int512_t) ("-4624572952211462269797505108208967248512037054815720126487996382735423935985812023649"),
                                (int512_t) ("4048712001891733945792569644632736493979276173586706840783426232846898667821381045226"),
                                (int512_t) ("4788308969329807836224317964185789457457693875245547205600060914548336905519427930489"),
                                (int512_t) ("-1530854577366381238603340842969457940648645404586346045684526537216518601407556771060"),
                                (int512_t) ("-3337122355324464452525815663584011028740612628095530633834334103894086214254780130610"),
                                (int512_t) ("486139732568683890408239395794935773139101935155695376039094494731113579630235745007"),
                                (int512_t) ("-4330944701285398905362042041382301153079679126286639933707555890949952884230662977591"),
                                (int512_t) ("4574309107379226124687300794725122710141446536464861059831807775618577915170081262837"),
                                (int512_t) ("7377882592950372070366798068659526646228275787867629180408057628440816650642196899972"),
                                (int512_t) ("-5011531690327550565017068384118742598242883485019330422563227644014008094644667235466"),
                                (int512_t) ("-7229080763305663566318433275820645739658724603440088863801138766004537944204471635519"),
                                (int512_t) ("-3855211486832153514799421838103500126375423238468936851477250592520160994237331413160"),
                                (int512_t) ("6665936102618869966646308680855214377850871517115481889479479880292225123870882016181"),
                                (int512_t) ("-1542651089070274135073903871155783582649177437574230676829587330778659237232098061746"),
                                (int512_t) ("5994483640047048782980130862316752171892854659929754731820755199960026713839129091496"),
                                (int512_t) ("-1627591417615552853141527959234668131691607393888041547069637048649923892049026850714"),
                                (int512_t) ("-4275002020770836708129900094774613064841713762403446826542881321019482447506468792007"),
                                (int512_t) ("6531517332657743751924231743925900287152323314237287546833339665373893021389099477964"),
                                (int512_t) ("-5260758489308905763117684945850773907849986561905105254646868574553508243918218219402"),
                                (int512_t) ("-3905897278039378399028038361988193398145125434625051126139764710057514227929836815897"),
                                (int512_t) ("-2084498941620302205452935509580721439135412200385726496665772623345170082805328114338"),
                                (int512_t) ("3147396894762561900503633166530539237048813232923899388286709309445248673669692644806"),
                                (int512_t) ("1955232137828822965991949488505902545995718283998616402649686158785897635105410985809"),
                                (int512_t) ("2731607813033288200754318639768015319343353612737280382954518489195364101365511667060"),
                                (int512_t) ("-2766064498988586502559117059414630246793915087435065584344710841889304395070006766140"),
                                (int512_t) ("7422678451091967638040631055127917882328917226550615950918106414312624269578431413909"),
                                (int512_t) ("7288050907065020253742906224436621465704011519737736912283228323111726864135154034609"),
                                (int512_t) ("-3579123273552851742356044077001532656449432312692746723749100717123060883276544418216"),
                                (int512_t) ("-340106933429627638550955400285140951636245125676039162478256929267558512658917011095"),
                                (int512_t) ("-6862784288424716452918122884937217986754905963684422159940011979367007489558081144819"),
                                (int512_t) ("-6407364503058612822321805870011451841711803732937498692828032239765261533106321053673"),
                                (int512_t) ("1451026777587473366720864710026050337989787938904033476604935119624788928309022459985"),
                                (int512_t) ("2776375490491394285743596749119698742505867461557068286058280323161126903472699143473"),
                                (int512_t) ("2312465944156621005903838555144324384352735533103680229868229688400701965777432035785"),
                                (int512_t) ("4927028222256281704468472896159022887980030021060439582296733021018337018843884030250"),
                                (int512_t) ("5392593198672430283885345710712339691988829768205266156157020717744205333344119384753"),
                                (int512_t) ("-3561496905435323670007212471824943844389447886610070427583690986970337668242914067178"),
                                (int512_t) ("3059327654734401428445565756810166808732161776276667186042514381367435963306951844869"),
                                (int512_t) ("1939842679116370454128265292851644861572833484181527740354911882665608883385390883559"),
                                (int512_t) ("4547230589317692589158721798013799777937898015042316548233682255061039355871392945373"),
                                (int512_t) ("3790779578806029944997624191074649547635618385006742709116765973479939614857784935875"),
                                (int512_t) ("509337080010027662726977903842059036279015080732074650995583156031040756595139533538"),
                                (int512_t) ("5360351928277078405121471654904457166807872972184909729805378618780527674367355297533"),
                                (int512_t) ("-1339413789988066806459028334792356248148831435428577088777247202454274310404546167925"),
                                (int512_t) ("-4463906696484270287214220127723029505762471364346661462227805101812555620886237799794"),
                                (int512_t) ("-2258093448928437233069483950410272772780946372509761726575206937622436568326150142025"),
                                (int512_t) ("1004256551930554353114296219838910310743909799528845501145864496085857104650314862812"),
                                (int512_t) ("319557745177315113563453368172240503749898557559177325756635163594610787280872496384"),
                                (int512_t) ("5185575858468699587389875226776943133972058236294322651481152399028371128866524880451"),
                                (int512_t) ("2204411101345708196524279338782432272903425536694469538958298880263790046574661667169"),
                                (int512_t) ("-3631937978423924039738277347992779691430854258049564988182695812322313322500320695058"),
                                (int512_t) ("-5300010279189585338293612737740112126813365287141555342367330113351296299634378167599"),
                                (int512_t) ("-4357660893340540523656994789330634228194592906656056283507596910391694844114969474194"),
                                (int512_t) ("-3737659811201698752224417271114634608581502314636317627695374765471483572731910849472"),
                                (int512_t) ("888850817258072505926506899396875533114207855902506433060590678007052885039792569425"),
                                (int512_t) ("5009213324672261387568099912542251153454373447687771532243539720972306294468007354009"),
                                (int512_t) ("-2381687254568798408374571437655085129321323028657170106787234360826668866427951238597"),
                                (int512_t) ("4191394326013823060988142219518344821982355877524479119134429684151512580348948605"),
                                (int512_t) ("-5946226829753369923178826092468335686933224997166559434449484367351922707268723254744"),
                                (int512_t) ("-2825171062271460531495032755085981218392154503423881826015974701139704580601933297285"),
                                (int512_t) ("-2816997809724109008921643081600969816090991617659675783089824074037109673843436601756"),
                                (int512_t) ("-1817149916058603037867457871513994899970935152294985809993925895000959158216394218809"),
                                (int512_t) ("4937156414617494068933680124295440412376663051770300895520420530458791313302466087307"),
                                (int512_t) ("-6379174577015161821593642185553653047312413405306690511413028577793603330937108201814"),
                                (int512_t) ("-1889518201050035654167016724331687624194453804432438302146748861248876507671959890166"),
                                (int512_t) ("6424876375093501922687649091628679965940005797505617998952735457214765581891809995418"),
                                (int512_t) ("-3314427065017514753656293178596479444175687311332058511024279216545758122724710460312"),
                                (int512_t) ("3762151537480563206661049652821970919947786582064952405964038373830148911676554013528"),
                                (int512_t) ("-121263884953027377905618378800414452961879304259592966833448723993557313882294474726"),
                                (int512_t) ("-7673001294574667315264622349742478118834075838733244060502550257385345485995269020973"),
                                (int512_t) ("-2163348664375322475354754596251998068827414791590257259725183354356601543210166676751"),
                                (int512_t) ("-4789603840578841469882276643340458061764955247103590770569081370540595524236809491269"),
                                (int512_t) ("7563647337782084201734319850762984950880098378880645556629704832526763264063306614176"),
                                (int512_t) ("-3606016685127478462141879162350127445715551327580937727265356246766854107721665775216"),
                                (int512_t) ("359161632438443353759671071576082504478009682256885704246852926491934500021665226866"),
                                (int512_t) ("-3992191421732162905236933599228126093464139369947755757027596005319632336990817779097"),
                                (int512_t) ("2077199145483052737746475277041833078511725794822793009496738783963466514298581013250"),
                                (int512_t) ("-6345462019138918779047790006222878568215814851830009016801336995613445364255859557263"),
                                (int512_t) ("4192018497112697195005417067534010352425342884109165497437505866446838873571584967180"),
                                (int512_t) ("-7660603818414246399601732237216232598642683099424061994266166038684917577733287250338"),
                                (int512_t) ("-3623840954950353361003145003123947079616169799643713261455011654819756135438483176635"),
                                (int512_t) ("-6863179626863847238012638672582832076688670362894101157809766547630057368189973988750"),
                                (int512_t) ("-705111922999624449725878236386944831966789223594947362763742351667388129176756712185"),
                                (int512_t) ("5099983503199522203478983935001973433749271125894476629158301595374510471725078076427"),
                                (int512_t) ("-579947972888001379964299970562928235925334330006334489295976555717362234255034422351"),
                                (int512_t) ("5565215683327387159812237409477516512399864702254358509931240657429840236000371305190"),
                                (int512_t) ("377489811127271488149123229232553470862361913836493341662806131005450514945869205391"),
                                (int512_t) ("6667397543580079167105855904495674858641129892622320287339131178438197042088925464266"),
                                (int512_t) ("-6946889868637669561485132764678387861319314765462086783829467634148586521551983184638"),
                                (int512_t) ("673964310539411258232557514617499563908444677062914451779740818341737655656874099915"),
                                (int512_t) ("965359447615907724676265634593525737453446537627557829432954274907380656100914097713"),
                                (int512_t) ("6827795503101884346136288020538432563028950286303142432829338269104038109144057899637"),
                                (int512_t) ("-4753440174524754801306754417642221096156256226240957625360738360416991519411619566962"),
                                (int512_t) ("-4629148418043940550257708050426279424264930819078885365497376562997156302166225843052"),
                                (int512_t) ("-3139202826795796264067169048054815736333452751223128883794236330478711043443294963749"),
                                (int512_t) ("-6032805525130278702690458887713472355145298893141537587838485342943850328215179920339"),
                                (int512_t) ("5212555300217467178347639291422903078652556511232221152893566548578473776473091297604"),
                                (int512_t) ("-3218432487972038243162324247592565474257310363725809661324505664887129938538812859823"),
                                (int512_t) ("5552315745065050367637083167664053124399244241092673345636212929385492720983599814260"),
                                (int512_t) ("5751245132105239886484714354780487782429424124631743987529868640694686650508154056784"),
                                (int512_t) ("-5986065689943380832775343054439982317515728920789897613176262422359532512078622614492"),
                                (int512_t) ("-1299519577182563999981305769467668145699461767000831221079290874661778210915423453630"),
                                (int512_t) ("-5128111248999410510555119594482376013410359107526008041776204504284265503076743367143"),
                                (int512_t) ("-2558420567598274769135788385104133633190680908639464182164936538942703982553769653102"),
                                (int512_t) ("7417272014081260474302904525288933927137170752729681128215807004558942213867005299489"),
                                (int512_t) ("6090900048199848808199336448451988482777265324229332115930998587385145297184361785757"),
                                (int512_t) ("-1109078490794357406369111448221762811102127867994224387769484003772527305002001373983"),
                                (int512_t) ("3591199565757442976756725196279721640436332353187051351579457997985116433133035805330"),
                                (int512_t) ("-2912339634063868051182656635090939319323204633947858077674256310425489172646792713142"),
                                (int512_t) ("3012889189723011322447438362144534529557579499141230764609398751740715731666939487763"),
                                (int512_t) ("6564919605558081875599099494741234809715030880600547587028073885086850692168289589022"),
                                (int512_t) ("1802081505673702395161852565208698969088989682213130712550540606353676478128828308662"),
                                (int512_t) ("-6529256430420530417824946197252053994807208103966209420653200231669586230265912635431"),
                                (int512_t) ("-5452519473564775271061121010120053621621225024106918603732736668678970935776479354912"),
                                (int512_t) ("1489600849108529467551751816135901392887521461037115414450811374664729792639671465458"),
                                (int512_t) ("-4078247363601152900680828241512760339144367895564265147752769208003622205600668579441"),
                                (int512_t) ("-6654532949654533687387546619844913476266175231971920280837966803274904902683607680530"),
                                (int512_t) ("3915844508882191609426246779599852829560489230782812330200634923810313356085098320514"),
                                (int512_t) ("-4750921037532158680681567302744561869856829772311597004200081463778188762918290688914"),
                                (int512_t) ("-4046335230417877475987089632739562658376763066841362882398722593404944492625612003886"),
                                (int512_t) ("-3434713300223411843693066207126019534506659349685825122572324486703671096863862592696"),
                                (int512_t) ("3952254965077159195579933231493205312684154167091456072241508422128338963732333897076"),
                                (int512_t) ("-480049794128508331119015498771273967213097857795477627023118398327823676927714234599"),
                                (int512_t) ("6873331181265062131973904756439923363723978293692985208595968482577572072067855799066"),
                                (int512_t) ("-4487008559220170619832588763377229711004927896035442179698092940826768186811872284691"),
                                (int512_t) ("-5106206580311775302844802013047385869870366799364661734797884010162791802892905020825"),
                                (int512_t) ("-1619744600338184212757905353478477811077462642427652652364619583416806009214454106012"),
                                (int512_t) ("6309459168033979799379324608439943514575667487544620706560590779115830414773491465293"),
                                (int512_t) ("-1093986302996704069076610892755226508589524272723587531853698563306918623371505252597"),
                                (int512_t) ("5701206614136206654872486608138288448912967605871373298047525927613872456713200504433"),
                                (int512_t) ("589073389206663682357202325476906858933455364692028640272743642075034400137922518513"),
                                (int512_t) ("-373925818702455132373451554416127344517467216860474736421650772259001061727056638801"),
                                (int512_t) ("-3184094583336250732265991285446858670898816407565778656306050057130376660298807097686"),
                                (int512_t) ("644516328507748100297265454634794154224818575611175027597421929168259967628283930182"),
                                (int512_t) ("-5245507922211310254975815024880276962174576032096331178399584752025704481367201480531"),
                                (int512_t) ("-7136034924524335238549634599099683016937226986164258346566463113937023068381055169312"),
                                (int512_t) ("-6689646852982786529530973791809836094446832365752298549169738595511722506096732670661"),
                                (int512_t) ("7586749490974598430417744143548086020768864128044204582549403298012740532165836379145"),
                                (int512_t) ("1884119082726086861875320802817606530079572426784928095316436469476138482104167082482"),
                                (int512_t) ("-481068481885022496248450111446481068006891561885170387462601430246691790969835012757"),
                                (int512_t) ("-7109040393716027937965504417505571585243765573872272197110898366119221518907523647028"),
                                (int512_t) ("-2351640470079428691942834977257229478948195953397677750746179307968146416595276201893"),
                                (int512_t) ("229244691873719429603352577012354396306630336777794998367586110510930792504267779242"),
                                (int512_t) ("7006303960348840846464581816578611565834026368899899008479680842137640069922307273534"),
                                (int512_t) ("1836394369245182866807205593826159454811126707880515973317382874189430232692962902177"),
                                (int512_t) ("6677971745741828299773027564626050060981309745141906238727080975482120127332448678922"),
                                (int512_t) ("-2388633925144592032500164881332987815940144232589881243673946067008744312568271258617"),
                                (int512_t) ("-1140716317540872635117996482951389991598053012898457331716360919874885741240331532252"),
                                (int512_t) ("-3349577339569879991081995973788706463444191432926525238707342681071788628640313697038"),
                                (int512_t) ("3058628372139718345146479051299376282074323977520845561664075890177065420823653817953"),
                                (int512_t) ("4208262054937508319340204031329135612130629988720523507191656201031263652459930641185")
};
#endif //ZKSC_PARAMS32_H